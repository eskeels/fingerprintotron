#include <dirent.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>

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

    for (size_t i = 0; i < docs.size(); ++i)
    {
        for (size_t j = i+1; j < docs.size(); ++j)
        {
//            std::cout << i  << " " << j << std::endl;
            std::shared_ptr<ComparisonResult> result(new ComparisonResult);
            docs[i]->Compare(*docs[j],*result);
            std::cout << result->GetNameFirst() << " - " << result->GetNameSecond() << " %" << result->GetPercentage() << std::endl;
        }
    }
/* 
    ComparisonResult result;

    docs[0]->Compare(*(docs[1]),result);
    std::stringstream ss;
    result.Dump(ss);
    std::cout << ss.str() << std::endl;
    std::cout << "---------------------" << std::endl;
    result.RemoveOverlaps();
    std::cout << "---------------------" << std::endl;
    std::stringstream ss2;
    result.Dump(ss2);
    std::cout << ss2.str() << std::endl;

    result.AnalyzeResults();
    std::cout << "Min consecutive hash " << result.GetMin() << std::endl
        << "Max consecutive hash " << result.GetMax()  << std::endl
        << "Total matching hashes " << result.GetTotal() << std::endl
        << "Percentage " << std::max(result.GetPercentageRHS(),result.GetPercentageLHS()) << "%" << std::endl;
*/
    return 0;
}

