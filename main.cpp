#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <set>

#include <unicode/uchriter.h>
#include <unicode/ustdio.h>
#include <unicode/utypes.h>
#include <unicode/uchar.h>

#include "hasher.h"
#include "ngramiterator.h"
#include "fingerprintgenerator.h"
#include "document.h"
#include "comparisonresult.h"

using namespace FingerPrintOTron;

int ParseCmdLine(int argc, char* argv[], std::vector<std::string>& fnames)
{
    if (argc<=1)
    {
        std::cerr << "Invalid command line options. Need directory or files to process." << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        for (size_t i = 1; i < argc ; ++i)
        {
            fnames.push_back(std::string(argv[i]));
        }
    }
    return 0;
}

int GetFilesToProcess(const std::string& dirName, std::vector<std::string>& filesToFP)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dirName.c_str())) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            std::string f(ent->d_name);
            if (!f.empty() && f[0] != '.')
            {
                filesToFP.push_back(dirName+"/"+f);
            }
        }
        closedir (dir);
    }
    else
    {
        std::cerr << "Failed to open " << dirName << std::endl; 
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }

    return 0;
}

void ReadFile(const std::string& filename, std::string& buffer)
{
    std::fstream inputFile(filename.c_str());
    for( std::string line; std::getline(inputFile,line); )
    {
        buffer.append(line);
    }
}

std::shared_ptr<Document> HashFile(const std::string& filename)
{
    Hasher H;
    std::string buffer;
    ReadFile(filename,buffer);
    FingerPrintGenerator<Hasher> fp(buffer.c_str(),10,9,H);
    std::shared_ptr<Document> doc(fp.GetDocument(filename));
    return doc;
}

class DocumentCollectionAnalyser
{
    public:
        DocumentCollectionAnalyser(std::vector<std::shared_ptr<Document> >& docs)
            : mDocuments(docs)
        {
        }

        std::shared_ptr<std::set<std::string> > FindFileSet(const std::string& first, const std::string& second)
        {
            if (mListFileSets.empty())
            {
                std::shared_ptr<std::set<std::string> > newFileSet(new std::set<std::string>());
                mListFileSets.push_back(newFileSet);

                return newFileSet;
            }
            else
            {
                for (auto it = mListFileSets.begin() ; it != mListFileSets.end(); ++it)
                {
                    std::set<std::string>& fileSet = *(*it);

                    if ( fileSet.end() != fileSet.find(first) || fileSet.end() != fileSet.find(second) )
                    {
                        return *it;
                    }
                    else
                    {
                        std::shared_ptr<std::set<std::string> > newFileSet(new std::set<std::string>());
                        mListFileSets.push_back(newFileSet);
                        return newFileSet;
                    }
                }
            }

            return NULL;
        }

        void RecordResult(ComparisonResult& cr)
        {
            std::cout << cr.GetNameFirst() << " - " << cr.GetNameSecond() << " %" << cr.GetPercentage() << std::endl;
            if (cr.GetPercentage() >= 20)
            {
                std::shared_ptr<std::set<std::string> > fileSet( FindFileSet(cr.GetNameFirst(), cr.GetNameSecond()) );
                if (fileSet)
                {
                    fileSet->insert( cr.GetNameFirst() );
                    fileSet->insert( cr.GetNameSecond() );
                }
            }
        }

        void AnalysePair(const Document& first, const Document& second)
        {
            std::shared_ptr<ComparisonResult> result(new ComparisonResult);
            first.Compare(second,*result);
            result->AnalyzeResults();
            RecordResult(*result);
        }

        void Analyse()
        {
            for (size_t i = 0; i < mDocuments.size(); ++i)
            {
                for (size_t j = i+1; j < mDocuments.size(); ++j)
                {
                    AnalysePair(*mDocuments[i], *mDocuments[j]);
               }
            }
        }

        void Dump()
        {
            std::cout << "Similar documents" << std::endl;
            for (auto it = mListFileSets.begin() ; it != mListFileSets.end(); ++it)
            {
                std::set<std::string>& fileSet = *(*it);
                for (auto fileIt = fileSet.begin(); fileIt != fileSet.end(); ++fileIt)
                {
                    std::cout << *fileIt << " , ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

    protected:
        std::vector<std::shared_ptr<Document> >& mDocuments;
        std::vector<std::shared_ptr<std::set<std::string> > > mListFileSets;
};

bool IsFile(const std::string& name)
{
    bool bExists;
    if (FILE *file = fopen(name.c_str(), "r"))
    {
        fclose(file);
        bExists = true;
    }
    else
    {
        bExists = false;
    }

    return bExists;
}

bool IsDir(const std::string& name)
{
    DIR *pDir;
    bool bExists = false;

    pDir = opendir (name.c_str());

    if (pDir != NULL)
    {
        bExists = true;    
        (void) closedir (pDir);
    }

    return bExists;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> fileNames;
    int ret = ParseCmdLine(argc, argv, fileNames);
    if (ret != 0)
    {
        return ret;
    }

    std::vector<std::string> filesToFP;

    for (size_t i = 0; i < fileNames.size() && ret == 0; ++i)
    {
        if (IsDir(fileNames[i]))
        {
            ret = GetFilesToProcess(fileNames[i], filesToFP);
        }
        else if (IsFile(fileNames[i]))
        {
            filesToFP.push_back(fileNames[i]);
        }
        else
        {
            std::cout << "Unknown parameter " << fileNames[i] << std::endl;
            ret = -1;
        }
    }

    if (ret != 0)
    {
        return ret;
    }

    std::vector<std::shared_ptr<Document> > docs;

    for (const std::string& f : filesToFP)
    {
        std::cout << "Processing file:" << f << std::endl;
        std::shared_ptr<Document> doc(HashFile(f));
        docs.push_back(doc);
    }

    DocumentCollectionAnalyser dca(docs);

    dca.Analyse();
    dca.Dump();

    return 0;
}

