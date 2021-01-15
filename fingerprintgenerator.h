// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
#include <iostream>
#include <vector>
#include <memory>

#include <unicode/uchriter.h>
#include <unicode/ustdio.h>
#include <unicode/utypes.h>

#include <unicode/uchar.h>

#include "ngramiterator.h"
#include "document.h"

namespace FingerPrintOTron
{
    template<typename HASHFUNCTION>
    class FingerPrintGenerator
    {
        public:
            // Takes a null terminated UTF-8
            FingerPrintGenerator(const char *UTF8Buffer, const uint16_t NGramSize, const uint16_t WinnowSize, HASHFUNCTION hashFunction)
                : mWINNOW_SIZE(WinnowSize),
                  mNGRAM_SIZE(NGramSize),
                  mW(0),
                  mMinHash(0),
                  mUS(icu::UnicodeString::fromUTF8(UTF8Buffer)),
                  mHashFunction(hashFunction)
            {
                const UChar *ucharTxt = mUS.getTerminatedBuffer();
                mNGI.reset(new NGramIterator(ucharTxt, u_strlen(ucharTxt), mNGRAM_SIZE));
            }

            // Takes UTF-32. len is the number of UTF32 characters.
            FingerPrintGenerator(const UChar32 *UTF32Buffer, const uint16_t len, const uint16_t NGramSize, const uint16_t WinnowSize, HASHFUNCTION hashFunction)
                : mWINNOW_SIZE(WinnowSize),
                  mNGRAM_SIZE(NGramSize),
                  mW(0),
                  mMinHash(0),
                  mUS(icu::UnicodeString::fromUTF32(UTF32Buffer,len)),
                  mHashFunction(hashFunction)
            {
                const UChar *ucharTxt = mUS.getTerminatedBuffer();
                mNGI.reset(new NGramIterator(ucharTxt, u_strlen(ucharTxt), mNGRAM_SIZE));
            }

            // Takes UTF-16. len is the number of UTF16 characters.
            FingerPrintGenerator(const UChar *UTF16Buffer, const uint16_t len, const uint16_t NGramSize, const uint16_t WinnowSize, HASHFUNCTION hashFunction)
                : mWINNOW_SIZE(WinnowSize),
                  mNGRAM_SIZE(NGramSize),
                  mW(0),
                  mMinHash(0),
                  mHashFunction(hashFunction)
            {
                mNGI.reset(new NGramIterator(UTF16Buffer, len, mNGRAM_SIZE));
            }

            // Records the minimum hash
            void SetMinHash(HASH hash)
            {
                // if in first winnow the use that hash
                if (mW == 0)
                {
                    mMinHash = hash;
                }
                else if (hash < mMinHash)
                {
                    // if new hash is smaller then use it
                    mMinHash = hash;
                }
            }

            // Gets the hash of the next NGram and
            // records it if its the minimum for the
            // WINNOW
            bool Next()
            {
                while (mNGI->Next())
                {
                    HASH hash = mHashFunction(mNGI->GetNGram());

                    SetMinHash(hash);

                    // check to see if we have processed enough
                    // NGrams
                    if (mW == mWINNOW_SIZE-1)
                    {
                        mW = 0;
                        return true;
                    }
                    else
                    {
                        ++mW;
                    }
                }

                return false;
            }

            // This algorithm uses the minimum
            // hash calculaed for the WINNOW
            HASH GetHash() const
            {
                return mMinHash;
            }

            // Once Next() has returned false this
            // will see if there is a left over hash
            // due to the buffer not being evenly
            // divisible by the WINNOW
            bool Leftover() const
            {
                return (mW != 0);
            }

            // Uses the finger printer generator to iterate through
            // the buffer and populate a document with the hashes
            std::shared_ptr<Document> GetDocument(const std::string& name)
            {
                std::shared_ptr<Document> doc(new Document(name));
                while (Next())
                {
                    doc->AddHash(GetHash());
                }
                if (Leftover())
                {
                    doc->AddHash(GetHash());
                }

                return doc;
            }

        protected:
            // WINNOW size
            const uint16_t mWINNOW_SIZE;
            // NGRAM size
            const uint16_t mNGRAM_SIZE;
            // NGRAM iterator
            std::shared_ptr<NGramIterator> mNGI;
            // The current WINNOW. Resets when
            // it gets to mWINNOW_SIZE
            uint16_t mW;
            // The minimum has for the WINNOW
            HASH mMinHash;
            // The ICU Unicode string for the input buffer
            icu::UnicodeString mUS;
            // The hash function
            HASHFUNCTION mHashFunction;
    };
}

