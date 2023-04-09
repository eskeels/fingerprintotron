// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "hasher.h"
#include "fingerprintgenerator.h"
#include "document.h"
#include "documentcollectionanalyser.h"

using namespace FingerPrintOTron;

int ParseCmdLine(int argc, char* argv[], std::vector<std::string>& fnames, std::map<std::string,std::string>& params)
{
    if (argc<=1)
    {
        std::cerr << "Invalid command line options. Need directory or files to process." << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "--threshold   - percentage of common signatures before 2 documents are considered as similar." << std::endl;
        std::cerr << "--ngram       - NGram size is the number of characters of a word that are hashed." << std::endl;
        std::cerr << "--winnow      - Winnow is the size of the window to select fingerprints from." << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        for (size_t i = 1; i < argc ; ++i)
        {
            std::string param(argv[i]);
            if (0 == param.compare(0,2,"--"))
            {
                ++i;
                if (i < argc)
                {
                    std::string value(argv[i]);
                    params.insert(std::make_pair(param,value));
                }
            }
            else
            {
                fnames.push_back(param);
            }
        }
    }
    return EXIT_SUCCESS;
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

std::shared_ptr<IDocument> HashFile(const std::string& filename, uint32_t NGramSize, uint32_t WinnowSize)
{
    Hasher H;
    std::string buffer;
    ReadFile(filename,buffer);
    FingerPrintGenerator<Hasher> fp(buffer.c_str(),NGramSize,WinnowSize, H);
    std::shared_ptr<IDocument> doc(new Document(filename));
    fp.Process(*doc);
    return doc;
}

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

int ProcessFiles(const std::vector<std::string>& fileNames, std::vector<std::string>& filesToFP)
{
    int ret = EXIT_SUCCESS;

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
            std::cerr << "Unknown parameter " << fileNames[i] << std::endl;
            ret = EXIT_FAILURE;
        }
    }

    if (ret != EXIT_SUCCESS)
    {
        return ret;
    }

    if (filesToFP.empty())
    {
        std::cerr << "No files found to process!" << std::endl;
        return EXIT_FAILURE;
    }

    return ret;
}

int ProcessParams(std::map<std::string,std::string>& params, uint32_t& NGramSize, uint32_t& WinnowSize, uint32_t& threshold)
{
    int ret = EXIT_SUCCESS;

    if (params.find("--ngramsize") != params.end())
    {
        NGramSize = static_cast<uint32_t>(std::stoul(params["--ngramsize"]));
    }

    if (params.find("--winnowsize") != params.end())
    {
        WinnowSize = static_cast<uint32_t>(std::stoul(params["--winnowsize"]));
    }

    if (params.find("--threshold") != params.end())
    {
        threshold = static_cast<uint32_t>(std::stoul(params["--threshold"]));
    }

    return ret;
}

int main(int argc, char* argv[])
{
    int ret = EXIT_SUCCESS;

    std::vector<std::string> fileNames;
    std::map<std::string,std::string> params;
    ret = ParseCmdLine(argc, argv, fileNames, params);
    if (ret != EXIT_SUCCESS)
    {
        return ret;
    }

    std::vector<std::string> filesToFP;
    ret = ProcessFiles(fileNames, filesToFP);
    if (ret != EXIT_SUCCESS)
    {
        return ret;
    }

    uint32_t NGramSize = 10;
    uint32_t WinnowSize = 9;
    uint32_t threshold = 20;

    ret = ProcessParams(params, NGramSize, WinnowSize, threshold); 
    if (ret != EXIT_SUCCESS)
    {
        return ret;
    }

    std::cout << "Comparing documents with the following parameters:" << std::endl;
    std::cout << "NGramSize = " << NGramSize << std::endl << "WinnowSize = " << WinnowSize << std::endl << "Threshold = " << threshold << std::endl;
    std::vector<std::shared_ptr<IDocument> > docs;
    std::cout << std::endl;
    std::cout << "Processing files:" << std::endl;
    for (const std::string& f : filesToFP)
    {
        std::cout << f << std::endl;
        std::shared_ptr<IDocument> doc(HashFile(f, NGramSize, WinnowSize));
        docs.push_back(doc);
    }
    std::cout << std::endl;
    DocumentCollectionAnalyser dca(docs, threshold);

    dca.Analyse();
    std::stringstream ss;
    dca.Dump(ss);
    std::cout << ss.str();

    return ret;
}

