#include <iostream>

void ASSERT_F(bool b, const char * file, int line)
{
    if (!b)
    {
        std::cout << "Failed on line " << line << " of " << file << "!" << std::endl;
        exit(1);
    }
}

#define ASSERT(a)(ASSERT_F(a,__FILE__,__LINE__))


