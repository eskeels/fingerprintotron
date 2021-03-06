// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
#pragma once

#include <vector>

#include <unicode/uchar.h>

#include "murmur3.h"

namespace FingerPrintOTron
{
    typedef uint16_t HASH;

    class Hasher
    {
        public:
        
        HASH operator()(const std::vector<UChar32>& word)
        {
            uint32_t seed = 42;
            uint32_t hash = 0;
            MurmurHash3_x86_32(&word[0], word.size() * sizeof(UChar32), seed, &hash);
            HASH hash16 = (HASH)hash;
            return hash16;
        }
    };
} 
