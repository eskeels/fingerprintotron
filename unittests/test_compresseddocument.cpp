#include "test.h"

#include "compresseddocument.h"

using namespace FingerPrintOTron;

void Test()
{
    std::vector<HASH> hashes { 128,2,3,4,3,5,6,99,123,64,1 };
    std::vector<HASH> expectedOdd { 1,3,5 };
    std::vector<HASH> expectedEven { 2,4,6 };

    CompressedDocument doc1("doc1.txt", 3);
    for ( auto h : hashes )
    {

        doc1.AddHash(h);
for (auto o : doc1.GetOdd()){
std::cout << o << " "; }
std::cout << std::endl;
 
    }
   ASSERT( expectedOdd == doc1.GetOdd() );
    ASSERT( expectedEven == doc1.GetEven() );
}

int main(int argc, char* argv[])
{
    Test();
    return 0;
}

