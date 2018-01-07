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
    FingerPrintGenerator<Hasher> fp(buffer.c_str(),NGRAM,WINNOWSIZE,H);
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

const std::string sherlock = "I had called upon my friend Sherlock Holmes upon the second morning"
                       " after Christmas, with the intention of wishing him the compliments of"
                       " the season. He was lounging upon the sofa in a purple dressing-gown,"
                       " a pipe-rack within his reach upon the right, and a pile of crumpled"
                       " morning papers, evidently newly studied, near at hand. Beside the couch"
                       " was a wooden chair, and on the angle of the back hung a very seedy"
                       " and disreputable hard-felt hat, much the worse for wear, and cracked"
                       " in several places. A lens and a forceps lying upon the seat of the"
                       " chair suggested that the hat had been suspended in this manner for the"
                       " purpose of examination.";

const std::string tale_of_2_cities = "At last, the top of the staircase was gained, and they stopped for the"
                                    " third time. There was yet an upper staircase, of a steeper inclination"
                                    " and of contracted dimensions, to be ascended, before the garret story"
                                    " was reached. The keeper of the wine-shop, always going a little in"
                                    " advance, and always going on the side which Mr. Lorry took, as though he"
                                    " dreaded to be asked any question by the young lady, turned himself about"
                                    " here, and, carefully feeling in the pockets of the coat he carried over"
                                    " his shoulder, took out a key.";

const std::string japanese = "大阪大（大阪府吹田市）は６日、２０１７年２月に実施した工学部や理学部など６学部の"
                             "一般入試（前期日程）の物理で出題と採点にミスがあり、本来なら合格していた３０人を"
                             "不合格にしていたと発表した。 　全員を追加合格とし、希望者は今年４月に１～２年生で"
                             "の転入学を認める。他大学や予備校に通うなどしているとみられ、授業料などの補償や慰"
                             "謝料の支払いを行う。大阪大は昨年、２度にわたり外部から「誤りがある」との指摘を受け"
                             "たにもかかわらず、適切な対応をしていなかった。";

const std::string japanese_line_4 = "の転入学を認める。他大学や予備校に通うなどしているとみられ、授業料などの補償や慰";

// compare same string
void TestSame()
{
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, sherlock);
    ASSERT(100 == cr->GetPercentageRHS());
    ASSERT(100 == cr->GetPercentageLHS());
}
// compare 2 different strings
void TestDifferent()
{
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, tale_of_2_cities);
    ASSERT(0 == cr->GetPercentageRHS());
    ASSERT(0 == cr->GetPercentageLHS());
}
// compare same string but one version is upper case and
// the other is lower case
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
// compare the same string but one character in the middle
// has been changed to a "."
void TestOneCharDiff()
{
    std::string sherlockOneCharDiff(sherlock);
    sherlockOneCharDiff[sherlockOneCharDiff.size()/2] = '.';
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, sherlockOneCharDiff);

    ASSERT(91 == cr->GetPercentageRHS());
    ASSERT(91 == cr->GetPercentageLHS());
}
// compare whole string to half
void TestHalfDiff()
{
    std::string sherlockHalf(sherlock,sherlock.size()/2);
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, sherlockHalf);

    ASSERT(92 == cr->GetPercentageRHS());
    ASSERT(45 == cr->GetPercentageLHS());
}
// create a string that has tale_of_2_cities with sherlock either side
void TestEncased()
{
    std::string str(sherlock);
    str.append(tale_of_2_cities);
    str.append(sherlock);

    {
    std::shared_ptr<ComparisonResult> cr = CompareText(sherlock, str);
    ASSERT(35 == cr->GetPercentageRHS());
    ASSERT(100 == cr->GetPercentageLHS());
    }

    {
    std::shared_ptr<ComparisonResult> cr = CompareText(tale_of_2_cities, str);
    ASSERT(21 == cr->GetPercentageRHS());
    ASSERT(76 == cr->GetPercentageLHS());
    }
}
// testing some multiple byte UTF-8
void TestJapaneseNoMatch()
{
    std::shared_ptr<ComparisonResult> cr = CompareText(japanese, sherlock);
    ASSERT(0 == cr->GetPercentage());
}

void TestJapaneseSame()
{
    std::shared_ptr<ComparisonResult> cr = CompareText(japanese, japanese);
    ASSERT(100 == cr->GetPercentage());
}

void TestJapaneseMatch()
{
    std::string str(sherlock);
    str.append(japanese_line_4);
    str.append(tale_of_2_cities);

    std::shared_ptr<ComparisonResult> cr = CompareText(japanese, str);
    ASSERT(18 == cr->GetPercentage());
}
int main(int argc, char* argv[])
{
    TestSame();
    TestDifferent();
    TestUpperLower();
    TestOneCharDiff();
    TestHalfDiff();
    TestEncased();
    TestJapaneseNoMatch();
    TestJapaneseSame();
    TestJapaneseMatch();
    return 0;
}

