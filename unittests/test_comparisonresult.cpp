#include "test.h"

#include "comparisonresult.h"
#include "document.h"

using namespace FingerPrintOTron;

void TestWithin()
{
    ComparisonResult res;
    ASSERT(false == res.Within(0,5,6,10));
    ASSERT(false == res.Within(0,6,6,10));
    ASSERT(false == res.Within(0,7,6,10));
    ASSERT(true == res.Within(0,10,6,10));
}

void TestEmpty()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;
    std::stringstream ss;
    result.Dump(ss);
    ASSERT( result.ZeroMatch() );
    result.AnalyzeResults();
    ASSERT(result.GetMin() == 0);
    ASSERT(result.GetMax() == 0);
    ASSERT(result.GetTotal() == 0);
    ASSERT(result.GetPercentageSecond() == 0);
    ASSERT(result.GetPercentageFirst() == 0);
}

void TestMatch1HashExact()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc2.AddHash(1);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    ASSERT(result.GetHash(0)->at(0) == 1);
    ASSERT(result.GetHash(1) == NULL);
    result.AnalyzeResults();

    ASSERT(result.GetMin() == 1);
    ASSERT(result.GetMax() == 1);
    ASSERT(result.GetTotal() == 1);
    ASSERT(result.GetPercentageSecond() == 100);
    ASSERT(result.GetPercentageFirst() == 100);
}

void TestMatch3HashExact()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    doc2.AddHash(1);
    doc2.AddHash(2);
    doc2.AddHash(3);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    ASSERT(result.GetHash(0)->at(0) == 1);
    ASSERT(result.GetHash(0)->at(1) == 2);
    ASSERT(result.GetHash(0)->at(2) == 3);
    result.AnalyzeResults();

    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 100);
    ASSERT(result.GetPercentageFirst() == 100);

}

void TestOverwrite()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    doc2.AddHash(1);
    doc2.AddHash(2);
    // second occurance causes "12" to be overwritten
    doc2.AddHash(1);
    doc2.AddHash(2);
    doc2.AddHash(3);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    ASSERT(result.GetHash(0)->at(0) == 1);
    ASSERT(result.GetHash(0)->at(1) == 2);
    ASSERT(result.GetHash(0)->at(2) == 3);

    result.AnalyzeResults();
    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 60);
    ASSERT(result.GetPercentageFirst() == 100);
}

void TestSkip()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    doc2.AddHash(1);
    doc2.AddHash(2);
    doc2.AddHash(3);
    // the second occurance of "12" is skipped
    doc2.AddHash(1);
    doc2.AddHash(2);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    ASSERT(result.GetHash(0)->at(0) == 1);
    ASSERT(result.GetHash(0)->at(1) == 2);
    ASSERT(result.GetHash(0)->at(2) == 3);
    result.RemoveOverlaps();
    result.AnalyzeResults();

    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 60);
    ASSERT(result.GetPercentageFirst() == 100);
}

void TestNoMatch()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    doc2.AddHash(4);
    doc2.AddHash(5);
    doc2.AddHash(6);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    ASSERT(result.ZeroMatch());
    result.RemoveOverlaps();
    result.AnalyzeResults();

    ASSERT(result.GetMin() == 0);
    ASSERT(result.GetMax() == 0);
    ASSERT(result.GetTotal() == 0);
    ASSERT(result.GetPercentageSecond() == 0);
    ASSERT(result.GetPercentageFirst() == 0);
}

void TestMatch2Hashes()
{
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);
    doc1.AddHash(4);

    doc2.AddHash(0);
    doc2.AddHash(2);
    doc2.AddHash(3);
    doc2.AddHash(0);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    ASSERT(result.GetHash(1)->at(0) == 2);
    ASSERT(result.GetHash(1)->at(1) == 3);
    result.RemoveOverlaps();

    result.AnalyzeResults();
    ASSERT(result.GetMin() == 2);
    ASSERT(result.GetMax() == 2);
    ASSERT(result.GetTotal() == 2);
    ASSERT(result.GetPercentageSecond() == 50);
    ASSERT(result.GetPercentageFirst() == 50);
}

void TestRemoveOverlap()
{
    {
    // 123
    //  23 - removed
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    doc2.AddHash(1);
    doc2.AddHash(2);
    doc2.AddHash(3);
   
    doc2.AddHash(2);
    doc2.AddHash(3);

    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    std::vector<HASH> expected = { 1, 2, 3 };
    ASSERT( *(result.GetHash(0)) == expected );

    result.AnalyzeResults();
    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 60);
    ASSERT(result.GetPercentageFirst() == 100);
    }

    {
    //
    // 023 - removed
    // 123
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    doc2.AddHash(2);
    doc2.AddHash(3);

    doc2.AddHash(1);
    doc2.AddHash(2);
    doc2.AddHash(3);
   
    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    std::vector<HASH> expected = { 1, 2, 3 };
    ASSERT( *(result.GetHash(0)) == expected );
    result.AnalyzeResults();

    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 60);
    ASSERT(result.GetPercentageFirst() == 100);
    }
}

void TestDoNotRemoveOverlap()
{
    {
    //
    // 1230
    // 0234 - not removed
    //
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);
    doc1.AddHash(0);

    doc2.AddHash(0);
    doc2.AddHash(2);
    doc2.AddHash(3);
    doc2.AddHash(4);
  
    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    std::vector<HASH> expected1 = { 2, 3 };
    std::vector<HASH> expected2 = { 0 };

    ASSERT( *(result.GetHash(1)) == expected1 );
    ASSERT( *(result.GetHash(3)) == expected2 );

    result.AnalyzeResults();
 
    ASSERT(result.GetMin() == 1);
    ASSERT(result.GetMax() == 2);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 75);
    ASSERT(result.GetPercentageFirst() == 75);
    }

    {
    // 0234 - not removed
    // 1230
    //
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");

    doc1.AddHash(std::vector<HASH>{0,2,3,4});
    doc2.AddHash(std::vector<HASH>{1,2,3,0});
  
    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;

    std::vector<HASH> expected0 = { 0 };
    std::vector<HASH> expected1 = { 2, 3 };

    ASSERT( *(result.GetHash(0)) == expected0 );
    ASSERT( *(result.GetHash(1)) == expected1 );
    result.AnalyzeResults(); 
    ASSERT(result.GetMin() == 1);
    ASSERT(result.GetMax() == 2);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 75);
    ASSERT(result.GetPercentageFirst() == 75);
    }
}

void TestWithinNextIt()
{
    // 12345 
    // 10
    // 11

    // 20 
    // 21
    // 234
    Document doc1("doc1.txt");
    Document doc2("doc2.txt");
    doc1.AddHash(std::vector<HASH>{1,2,3,4,5});
    doc1.AddHash(std::vector<HASH>{10,11});

    doc2.AddHash(std::vector<HASH>{20,21});
    doc2.AddHash(std::vector<HASH>{2,3,4});

    std::vector<HASH> expected0 = { 2,3,4 };

    {
    auto cr = doc1.Compare(doc2);
    ComparisonResult& result = (ComparisonResult&) *cr;
    ASSERT( *(result.GetHash(1)) == expected0 );
    result.AnalyzeResults();
    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 60);
    ASSERT(result.GetPercentageFirst() == 42);
    }

    {
    auto cr = doc2.Compare(doc1);
    ComparisonResult& result = (ComparisonResult&) *cr;
    ASSERT( *(result.GetHash(2)) == expected0 );
    result.AnalyzeResults();
    ASSERT(result.GetMin() == 3);
    ASSERT(result.GetMax() == 3);
    ASSERT(result.GetTotal() == 3);
    ASSERT(result.GetPercentageSecond() == 42);
    ASSERT(result.GetPercentageFirst() == 60);
    }
}

int main(int argc, char* argv[])
{
    TestEmpty();
    TestMatch1HashExact();
    TestMatch3HashExact();
    TestOverwrite();
    TestSkip();
    TestNoMatch();
    TestMatch2Hashes();
    TestWithin();
    TestRemoveOverlap();
    TestDoNotRemoveOverlap();
    TestWithinNextIt();

    return 0;
}

