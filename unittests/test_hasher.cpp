#include "test.h"

#include "hasher.h"

using namespace FingerPrintOTron;

void TestEmpty()
{
    std::vector<UChar32> word;
    Hasher hasher;
    ASSERT( 52572 == hasher(word) );
}

int main(int argc, char* argv[])
{
    TestEmpty();
    return 0;
}

