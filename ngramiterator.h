// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
#pragma once

#include <iostream>
#include <vector>

#include <unicode/uchriter.h>
#include <unicode/uchar.h>

namespace FingerPrintOTron
{
    class NGramIterator
    {
        public:
            NGramIterator(const UChar* text, int32_t size, size_t ngramSize)
                : mIter(text,size),
                  mNGramSize(ngramSize)
            {
                UChar32 c = mIter.first32();
                StoreChar(c);
            }

            void StoreChar(UChar32 c)
            {
                if (IsAlphaNumeric(c))
                {
                    mNGram.push_back(FoldCase(c));
                }
            }

            const std::vector<UChar32>& GetNGram() const
            {
                return mNGram;
            }

            bool Next()
            {
                if (mNGram.size() == mNGramSize)
                {
                    mNGram.erase(mNGram.begin());
                }
                UChar c;
                do
                {
                    c = mIter.next32();
                    StoreChar(c);
                } while(c != icu::CharacterIterator::DONE && mNGram.size() < mNGramSize);

                return(c != icu::CharacterIterator::DONE);
            }

            bool IsAlphaNumeric(UChar32 c) const
            {
                return u_isalnum(c);
            }

            UChar32 FoldCase(UChar32 c) const
            {
                return u_foldCase(c,U_FOLD_CASE_DEFAULT);
            }

        protected:
            icu::UCharCharacterIterator mIter; 
            size_t mNGramSize;
            std::vector<UChar32> mNGram;
    };
}
