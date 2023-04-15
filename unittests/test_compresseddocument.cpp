#include "test.h"

#include "compresseddocument.h"

using namespace FingerPrintOTron;

void TestAllOdd()
{
    std::vector<HASH> hashes { 31,9,7,17,13,3,3,3 };
    std::vector<HASH> expectedOdd { 3,7,9,13,17 };

    CompressedDocument doc1("doc1.txt", 5);
    for ( auto h : hashes )
    {
        doc1.AddHash(h);
    }

    ASSERT( expectedOdd == doc1.GetOdd() );
    ASSERT_EQ( 0, doc1.GetEven().size() );
}

void TestAllEven()
{
    std::vector<HASH> hashes { 128,2,4,6,64,14 };
    std::vector<HASH> expectedEven { 2,4,6 };

    CompressedDocument doc1("doc1.txt", 3);
    for ( auto h : hashes )
    {
        doc1.AddHash(h);
    }

    ASSERT_EQ( 0, doc1.GetOdd().size() );
    ASSERT( expectedEven == doc1.GetEven() );
}

void Test()
{
    std::vector<HASH> hashes { 128,2,3,4,3,5,6,99,123,64,1 };
    std::vector<HASH> expectedOdd { 1,3,5 };
    std::vector<HASH> expectedEven { 2,4,6 };

    CompressedDocument doc1("doc1.txt", 3);
    for ( auto h : hashes )
    {
        doc1.AddHash(h);
    }

    ASSERT( expectedOdd == doc1.GetOdd() );
    ASSERT( expectedEven == doc1.GetEven() );
}

int main(int argc, char* argv[])
{
    TestAllOdd();
    TestAllEven();
    Test();
    return 0;
}

