#include "test.h"

#include "hasher.h"

using namespace FingerPrintOTron;

void TestEmpty()
{
    std::vector<UChar32> word;
    Hasher hasher;
#ifdef HASH64
    ASSERT_EQ( 2390216701683322522U, hasher(word) );
#else
    ASSERT_EQ( 142593372U, hasher(word) );
#endif
}

int main(int argc, char* argv[])
{
    TestEmpty();
    return 0;
}

