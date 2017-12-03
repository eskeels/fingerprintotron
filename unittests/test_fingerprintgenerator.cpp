#include "test.h"

#include "fingerprintgenerator.h"

using namespace FingerPrintOTron;

class TestFPG : public FingerPrintGenerator
{
    public:
        TestFPG(const char *UTF8Buffer, const uint16_t NGramSize, const uint16_t WinnowSize)
            : FingerPrintGenerator(UTF8Buffer, NGramSize, WinnowSize)
        {
        }

        HASH GenerateHash(const std::vector<UChar32>& word) const
        {
            std::string h(word.begin(), word.end());
            mNGrams.push_back(h);
            return static_cast<HASH>(h[0]);
        }

    mutable std::vector<std::string> mNGrams;
};

void TestSimple()
{
    const char testChars[] = "abc def";
    // NGgram size of 3 so these are the expected NGrams
    std::vector<std::string> ngrams { "abc", "bcd", "cde", "def" };
    // Test hash function returns first character so expected
    // "hashes" are simply a and c
    std::vector<HASH> result { 'a', 'c' };
    TestFPG fp(testChars,3,2);
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
    TestFPG fp(testChars,3,2);
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
void TestSigMod()
{
    const char testChars[] = "A do run run run, a do run run";
    std::vector<HASH> result = { 33081, 5729, 7412, 33081, 5729 };
    FingerPrintGenerator fp(testChars,5,4);
    int i = 0;
    while (fp.Next())
    {
        ASSERT( result[i] == fp.GetHash() );
        i++;
    }
    ASSERT( result[i] == fp.GetHash() );
}

int main(int argc, char* argv[])
{
    TestSigMod();
    TestSimple();
    TestLeftover();
    return 0;
}
