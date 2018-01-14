#include <dirent.h>

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

int ParseCmdLine(int argc, char* argv[], std::string& dirName)
{
    if (argc!=2)
    {
        std::cerr << "Invalid command line options. Need directory to process." << std::endl;
        return EXIT_FAILURE;
    }

    dirName.assign(argv[1]);

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
            std::shared_ptr<std::set<std::string> > fileSet( FindFileSet(cr.GetNameFirst(), cr.GetNameSecond()) );
            if (fileSet)
            {
                fileSet->insert( cr.GetNameFirst() );
                fileSet->insert( cr.GetNameSecond() );
            }
        }

        void AnalysePair(const Document& first, const Document& second)
        {
            std::shared_ptr<ComparisonResult> result(new ComparisonResult);
            first.Compare(second,*result);
            result->AnalyzeResults();
//            std::cout << result->GetNameFirst() << " - " << result->GetNameSecond() << " %" << result->GetPercentage() << std::endl;
            if (result->GetPercentage() >= 20)
            {
                RecordResult(*result);
            }
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

int main(int argc, char* argv[])
{
    std::string dirName;
    int ret = ParseCmdLine(argc, argv, dirName);
    if (ret != 0)
    {
        return ret;
    }

    std::vector<std::string> filesToFP;

    ret = GetFilesToProcess(dirName, filesToFP);
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

