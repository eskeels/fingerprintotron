#include <algorithm>

#include "test.h"

#include "hasher.h"
#include "fingerprintgenerator.h"
#include "document.h"
#include "comparisonresult.h"

using namespace FingerPrintOTron;

const uint16_t NGRAM = 10; 
const uint16_t WINNOWSIZE = 9;

using namespace FingerPrintOTron;

std::shared_ptr<Document> HashText(const std::string& buffer)
{
    Hasher H;
    FingerPrintGenerator<Hasher> fp(buffer.c_str(),10,9,H);
    std::shared_ptr<Document> doc(fp.GetDocument("filename"));
    return doc;
}

std::shared_ptr<ComparisonResult> CompareText(const std::string& buffer1, const std::string& buffer2)
{
    std::shared_ptr<Document> doc1(HashText(buffer1));
    std::shared_ptr<Document> doc2(HashText(buffer2));

    std::shared_ptr<ComparisonResult> result(new ComparisonResult);

    doc1->Compare(*doc2,*result);

    result->AnalyzeResults();
    return result; 
}

std::string sherlock = "I had called upon my friend Sherlock Holmes upon the second morning"
                       " after Christmas, with the intention of wishing him the compliments of"
                       " the season. He was lounging upon the sofa in a purple dressing-gown,"
                       " a pipe-rack within his reach upon the right, and a pile of crumpled"
                       " morning papers, evidently newly studied, near at hand. Beside the couch"
                       " was a wooden chair, and on the angle of the back hung a very seedy"
                       " and disreputable hard-felt hat, much the worse for wear, and cracked"
                       " in several places. A lens and a forceps lying upon the seat of the"
                       " chair suggested that the hat had been suspended in this manner for the"
                       " purpose of examination.";


void TestSame()
{
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, sherlock);
    ASSERT(100 == cr->GetPercentageRHS());
    ASSERT(100 == cr->GetPercentageLHS());
}

void TestUpperLower()
{
    std::string sherlockUpper(sherlock);
    std::string sherlockLower(sherlock);

    std::transform(sherlockUpper.begin(), sherlockUpper.end(), sherlockUpper.begin(), ::toupper); 
    std::transform(sherlockLower.begin(), sherlockLower.end(), sherlockLower.begin(), ::tolower); 
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, sherlock);
    ASSERT(100 == cr->GetPercentageRHS());
    ASSERT(100 == cr->GetPercentageLHS());
}

int main(int argc, char* argv[])
{
    TestSame();
    TestUpperLower();
    return 0;
}

