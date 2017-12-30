// Public Domain kick in it to ya

//#include <dirent.h>

#include <iostream>
#include <fstream>

#include "hasher.h"
#include "fingerprintgenerator.h"
#include "document.h"
#include "comparisonresult.h"

using namespace FingerPrintOTron;

const uint16_t NGRAM = 10; 
const uint16_t WINNOWSIZE = 9;

int ParseCmdLine(int argc, char* argv[], std::string& firstFile, std::string& secondFile)
{
    if (argc!=3)
    {
        std::cerr << "Invalid command line options. Need 2 UTF-8 files to compare." << std::endl;
        return EXIT_FAILURE;
    }

    firstFile.assign(argv[1]);
    secondFile.assign(argv[2]);

    return EXIT_SUCCESS;
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
    std::string file1,file2;

    int ret = ParseCmdLine(argc, argv, file1, file2);
    if (ret != EXIT_SUCCESS)
    {
        return ret;
    }

    std::shared_ptr<Document> doc1(HashFile(file1));
    std::shared_ptr<Document> doc2(HashFile(file2));

    ComparisonResult result;

    doc1->Compare(*doc2,result);

    std::stringstream ss;
    result.Dump(ss);
    std::cout << ss.str() << std::endl;

    result.AnalyzeResults();
    std::cout << "Min consecutive hash " << result.GetMin() << std::endl
        << "Max consecutive hash " << result.GetMax()  << std::endl
        << "Total matching hashes " << result.GetTotal() << std::endl
        << "Percentage " << result.GetPercentage() << "%" << std::endl;

    return 0;
}

