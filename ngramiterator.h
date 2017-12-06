#pragma once
// Public Domain kick in it to ya

#include <iostream>
#include <vector>

#include <unicode/uchriter.h>
#include <unicode/uchar.h>

namespace FingerPrintOTron
{
    class NGramIterator
    {
        public:
            NGramIterator(const UChar* text, int16_t size, size_t ngramSize)
                : mIter(text,size),
                  mNGramSize(ngramSize)
            {
                UChar c = mIter.first();
                StoreChar(c);
            }

            void StoreChar(UChar c)
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
                    c = mIter.next();
                    StoreChar(c);
                } while(c != CharacterIterator::DONE && mNGram.size() < mNGramSize);

                return(c != CharacterIterator::DONE);
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
            UCharCharacterIterator mIter; 
            size_t mNGramSize;
            std::vector<UChar32> mNGram;
    };
}
