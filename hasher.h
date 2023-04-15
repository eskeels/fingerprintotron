#pragma once

#include <vector>

#include <unicode/uchar.h>

#include "murmur3.h"

#define HASH64

namespace FingerPrintOTron
{
#ifdef HASH32
    typedef uint32_t HASH;
#else
    typedef uint64_t HASH;
#endif

    class Hasher
    {
        public:

#ifdef HASH64        
        HASH operator()(const std::vector<UChar32>& word)
        {
            uint32_t seed = 42;
            uint64_t hash[2];
            MurmurHash3_x64_128(&word[0], word.size() * sizeof(UChar32), seed, &hash);
            return (HASH)(hash[0]^hash[1]);
        }
#endif

#ifdef HASH32
        HASH operator()(const std::vector<UChar32>& word)
        {
            uint32_t seed = 42;
            uint32_t hash = 0;
            MurmurHash3_x86_32(&word[0], word.size() * sizeof(UChar32), seed, &hash);
            return hash;
        }
#endif
    };
} 
