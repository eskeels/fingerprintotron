#include "test.h"

#include "ngramiterator.h"

using namespace FingerPrintOTron;

void TestSigMod()
{
    const std::string p("A do run run run, a do run run");
    UnicodeString us(UnicodeString::fromUTF8(p));
    NGramIterator NGI(us.getBuffer(), us.length(), 5);

    std::vector<std::string> result = { "adoru","dorun","orunr","runru","unrun","nrunr","runru","unrun","nruna","runad","unado","nador","adoru","dorun",
                                        "orunr","runru","unrun" };
    int i = 0;
    while(NGI.Next())
    {
        std::string test(NGI.GetNGram().begin(),NGI.GetNGram().end());
        ASSERT(test == result[i++]);
    }
}

void Test2NGram()
{
    const std::string p("12 3 45  6");
    UnicodeString us(UnicodeString::fromUTF8(p));
    NGramIterator NGI(us.getBuffer(), us.length(), 2);
    std::vector<std::string> result = { "12","23","34","45","56" };
    int i = 0;
    while(NGI.Next())
    {
        std::string test(NGI.GetNGram().begin(),NGI.GetNGram().end());
        ASSERT(test == result[i++]);
    }
}


void Test3NGram()
{
    const std::string p("123 456");
    UnicodeString us(UnicodeString::fromUTF8(p));
    NGramIterator NGI(us.getBuffer(), us.length(), 3);
    std::vector<std::string> result = { "123","234","345","456" };
    int i = 0;
    while(NGI.Next())
    {
        std::string test(NGI.GetNGram().begin(),NGI.GetNGram().end());
        ASSERT(test == result[i++]);
    }
}

void Test3NGramNoAlpha()
{
    const std::string p("!\" @###?? //// ()()** && %%% =====++++- {}");
    UnicodeString us(UnicodeString::fromUTF8(p));
    NGramIterator NGI(us.getBuffer(), us.length(), 3);
    ASSERT(false == NGI.Next());
    ASSERT(NGI.GetNGram().empty());
}

void Test3NGramJp()
{
    const std::string p("最新のニュース");
    UnicodeString us(UnicodeString::fromUTF8(p));
    NGramIterator NGI(us.getBuffer(), us.length(), 3);
    std::vector<std::string> result = { "最新の","新のニ","のニュ","ニュー","ュース" };
    int i = 0;
    while(NGI.Next())
    {
        UnicodeString us(UnicodeString::fromUTF32(&(NGI.GetNGram()[0]),NGI.GetNGram().size()));
        std::string test;
        us.toUTF8String(test);
        ASSERT(test == result[i++]);
    }
}

void Test3NGramJpMix()
{
    const std::string p("a最新のニ£ュ$bースc");
    UnicodeString us(UnicodeString::fromUTF8(p));
    NGramIterator NGI(us.getBuffer(), us.length(), 3);
    std::vector<std::string> result = { "a最新","最新の","新のニ","のニュ","ニュb","ュbー","bース","ースc" };
    int i = 0;
    while(NGI.Next())
    {
        UnicodeString us(UnicodeString::fromUTF32(&(NGI.GetNGram()[0]),NGI.GetNGram().size()));
        std::string test;
        us.toUTF8String(test);
        ASSERT(test == result[i++]);
    }
}

int main(int argc, char* argv[])
{
    TestSigMod();
    Test2NGram();
    Test3NGram();
    Test3NGramNoAlpha();
    Test3NGramJp();
    Test3NGramJpMix();
    return 0;
}

