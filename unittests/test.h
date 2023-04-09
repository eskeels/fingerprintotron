#include <iostream>

void ASSERT_F(bool b, const char * file, int line)
{
    if (!b)
    {
        std::cout << "Failed on line " << line << " of " << file << "!" << std::endl;
        exit(1);
    }
}

template<typename T, typename T2>
void ASSERT_EQ_F(T a, T2 b, const char * file, int line)
{
    if (a != b)
    {
        std::cout << a << "!=" << b <<" : Failed on line " << line << " of " << file << "!" << std::endl;
        exit(1);
    }
}


#define ASSERT(a)(ASSERT_F(a,__FILE__,__LINE__))
#define ASSERT_EQ(a,b)(ASSERT_EQ_F(a,b,__FILE__,__LINE__))


