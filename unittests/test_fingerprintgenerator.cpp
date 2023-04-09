#include <unicode/uchar.h>

#include "test.h"

#include "fingerprintgenerator.h"

using namespace FingerPrintOTron;

class TestHasher
{
    public:
    HASH operator()(const std::vector<UChar32>& word) const
    {
        std::string h(word.begin(), word.end());
        return static_cast<HASH>(h[0]);
    }
};

void TestSimple()
{
    const char testChars[] = "abc def";
    // NGgram size of 3 so these are the expected NGrams
    std::vector<std::string> ngrams { "abc", "bcd", "cde", "def" };
    // Test hash function returns first character so expected
    // "hashes" are simply a and c
    std::vector<HASH> result { 'a', 'c' };
    FingerPrintGenerator<TestHasher> fp(testChars,3,2,TestHasher());
    int i = 0;
    while (fp.Next())
    {
        ASSERT( result[i] == fp.GetHash() );
        i++;
    }
    // Winnow size is 2 so there are NO hashes left over
    ASSERT( false == fp.Leftover() );
}

void TestLeftover()
{
    const char testChars[] = "abc def h";
    // NGgram size of 3 so these are the expected NGrams
    std::vector<std::string> ngrams { "abc", "bcd", "cde", "def", "efh" };
    // Test hash function returns first character so expected
    // "hashes" are simply a and c
    std::vector<HASH> result { 'a', 'c' };
    FingerPrintGenerator<TestHasher> fp(testChars,3,2,TestHasher());
    int i = 0;
    while (fp.Next())
    {
        ASSERT( result[i] == fp.GetHash() );
        i++;
    }
    // Winnow size is 2 so there is a left over hash from the efh
    ASSERT( true == fp.Leftover() );
    ASSERT( 'e' == fp.GetHash());
}

#define TEST_CHARS "A do run run run, a do run run"
void TestSigMod()
{
    const char testChars[] = TEST_CHARS;
#ifdef HASH64
    std::vector<HASH> result = { 6686262350632615635,
                                 2525989291511638577,
                                 4574989650928438401,
                                 6686262350632615635,
                                 3986842452785137084 };
#else
    std::vector<HASH> result = { 33081, 5729, 7412, 33081, 5729 };
#endif
    FingerPrintGenerator<Hasher> fp(testChars,5,4,Hasher());
    int i = 0;
    while (fp.Next())
    {
        ASSERT_EQ( result[i] , fp.GetHash() );
        i++;
    }
    ASSERT_EQ( result[i] , fp.GetHash() );
}

void TestSigModWide()
{
    const std::string testChars = TEST_CHARS;
    icu::UnicodeString us(icu::UnicodeString::fromUTF8(testChars.c_str()));
    const UChar *ucharTxt = us.getTerminatedBuffer();
#ifdef HASH64
    std::vector<HASH> result = { 6686262350632615635,
                                 2525989291511638577,
                                 4574989650928438401,
                                 6686262350632615635,
                                 3986842452785137084 };
#else
    std::vector<HASH> result = { 33081, 5729, 7412, 33081, 5729 };
#endif

    FingerPrintGenerator<Hasher> fp(ucharTxt,testChars.size(),5,4,Hasher());
    int i = 0;
    while (fp.Next())
    {
        ASSERT_EQ( result[i] , fp.GetHash() );
        i++;
    }
    ASSERT_EQ( result[i] , fp.GetHash() );
}

int main(int argc, char* argv[])
{
    TestSigModWide();
    TestSigMod();
    TestSimple();
    TestLeftover();
    return 0;
}
