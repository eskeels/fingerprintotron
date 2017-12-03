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

int ParseCmdLine(int argc, char* argv[], std::string& firstFile, std::string& secondFile)
{
    if (argc!=3)
    {
        std::cerr << "Invalid command line options. Need 2 files to compare." << std::endl;
        return EXIT_FAILURE;
    }

    firstFile.assign(argv[1]);
    secondFile.assign(argv[2]);

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

int main(int argc, char* argv[])
{
    std::string file1,file2;

    int ret = ParseCmdLine(argc, argv, file1, file2);
    if (ret != 0)
    {
        return ret;
    }

    std::string buffer;
    ReadFile(file1,buffer);
    FingerPrintGenerator fp1(buffer.c_str(),10,9);
    std::shared_ptr<Document> doc1(fp1.GetDocument(file1));

    buffer.clear();
    ReadFile(file2,buffer);
    FingerPrintGenerator fp2(buffer.c_str(),10,9);
    std::shared_ptr<Document> doc2(fp2.GetDocument(file2));
 
    ComparisonResult result;

    doc1->Compare(*doc2,result);

    std::stringstream ss;
    result.Dump(ss);
    std::cout << ss.str() << std::endl;

    result.AnalyzeResults();
    std::cout << "Min consecutive hash " << result.GetMin() << std::endl
        << "Max consecutive hash " << result.GetMax()  << std::endl
        << "Total matching hashes " << result.GetTotal() << std::endl
        << "Percentage " << std::max(result.GetPercentageRHS(),result.GetPercentageLHS()) << "%" << std::endl;

    return 0;
}

