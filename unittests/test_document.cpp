#include "test.h"

#include "document.h"

using namespace FingerPrintOTron;

void TestEmpty()
{
    Document doc1("doc1.txt");
    ASSERT(doc1.Find(1) == NULL);
}

void Test1()
{
    Document doc1("doc1.txt");
    doc1.AddHash(1);
    doc1.AddHash(1);

    ASSERT(doc1.Find(1)->size() == 2);
    ASSERT(doc1.Find(1)->at(0) == 0);
    ASSERT(doc1.Find(1)->at(1) == 1);
}

void Test123()
{
    Document doc1("doc1.txt");
    doc1.AddHash(1);
    doc1.AddHash(2);
    doc1.AddHash(3);

    ASSERT(doc1.Find(1)->size() == 1);
    ASSERT(doc1.Find(1)->at(0) == 0);
    ASSERT(doc1.Find(2)->size() == 1);
    ASSERT(doc1.Find(2)->at(0) == 1);
    ASSERT(doc1.Find(3)->size() == 1);
    ASSERT(doc1.Find(3)->at(0) == 2);
    ASSERT(doc1.Find(4) == NULL);
}

int main(int argc, char* argv[])
{
    TestEmpty();
    Test1();
    Test123();
    return 0;
}

