#include <iostream>
#include <vector>
#include <memory>

#include <unicode/uchriter.h>
#include <unicode/ustdio.h>
#include <unicode/utypes.h>
// Public Domain kick in it to ya

#include <unicode/uchar.h>

#include "hasher.h"
#include "ngramiterator.h"
#include "document.h"

namespace FingerPrintOTron
{
    template<typename HASHFUNCTION>
    class FingerPrintGenerator
    {
        public:
            // Takes UTF-8
            FingerPrintGenerator(const char *UTF8Buffer, const uint16_t NGramSize, const uint16_t WinnowSize, HASHFUNCTION hashFunction)
                : mWINNOW_SIZE(WinnowSize),
                  mNGRAM_SIZE(NGramSize),
                  mW(0),
                  mMinHash(0),
                  mUS(UnicodeString::fromUTF8(UTF8Buffer)),
                  mHashFunction(hashFunction)
            {
                const UChar *ucharTxt = mUS.getTerminatedBuffer();
                mNGI.reset(new NGramIterator(ucharTxt, u_strlen(ucharTxt), mNGRAM_SIZE));
            }

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

            bool Next()
            {
                while (mNGI->Next())
                {
//                    HASH hash = GenerateHash(mNGI->GetNGram());
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
/*
            virtual HASH GenerateHash(const std::vector<UChar32>& word) const
            {
                return Hasher::GenerateHash(word);
            }
*/
            HASH GetHash() const
            {
                return mMinHash;
            }

            // Once Next() has returned false this
            // will see if there is a left over hash
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
            const uint16_t mWINNOW_SIZE;
            const uint16_t mNGRAM_SIZE;
            std::shared_ptr<NGramIterator> mNGI;
            uint16_t mW;
            HASH mMinHash;
            UnicodeString mUS;
            HASHFUNCTION mHashFunction;
    };
}

