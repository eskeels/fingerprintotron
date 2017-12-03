#include "test.h"

#include "hasher.h"

using namespace FingerPrintOTron;

void TestEmpty()
{
    std::vector<UChar32> word;
    ASSERT( 52572 == Hasher::GenerateHash(word) );
}

int main(int argc, char* argv[])
{
    TestEmpty();
    return 0;
}

