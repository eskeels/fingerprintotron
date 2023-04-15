#pragma once

#include <vector>
#include <map>
#include <memory>
#include <algorithm>

#include "hasher.h"
#include "icomparisonresult.h"
#include "idocument.h"
#include "compressedcomparisonresult.h"

namespace FingerPrintOTron
{
    // Document - Holds a map of HASH to position. The Hashes are stored in the
    // order they were calculated. Its not the position in the orginal document,
    // AddHash() simply increments a counter as they are added to preserve the
    // order. Document can compare to another document and the result goes into
    // ComparisonResult using the Compare method. Compare iterates through the
    // hashes of the document to compare against and finds them in this document.
    // If a hash is found then it checks forward for a sequence and stores that in
    // a vector which is added to the ComparisionResult.
    class CompressedDocument : public IDocument
    {
        public:
        CompressedDocument(const std::string& name, uint32_t size)
            : mName(name),
              mSize(size)
        {
        }

        ~CompressedDocument() {};
        
        void AddHash(HASH hash)
        {
            std::vector<HASH>* pV;
            if (hash % 2 == 0)
            {
                pV = &mEven;
            }
            else
            {
                pV = &mOdd;
            }

            // check for duplicate
            if (std::binary_search(pV->begin(), pV->end(), hash))
            {
                return;
            }

            if (pV->size() <= mSize)
            {
                pV->push_back(hash);
            }
            else
            {
                // overwrite the last entry
                (*pV)[mSize+1] = hash;
            }

            std::sort(pV->begin(), pV->end());
        }

        const std::vector<HASH>& GetOdd() const
        {
            return mOdd;
        }

        const std::vector<HASH>& GetEven() const
        {
            return mEven;
        }

        void Dump() const
        {
            std::cout << mName << std::endl;
            for (auto o : mOdd)
            {
                std::cout << o << std::endl;
            }
            std::cout << std::endl;
            for (auto e : mEven)
            {
                std::cout << e << std::endl;
            }
        }

        void FindMatching(const std::vector<HASH>& v1, const std::vector<HASH>& v2, CompressedComparisonResult& cr) const
        {
            // NB ignore the last entry in the vector
            for( int i = 0 ; i < v1.size()-1 ; ++i )
            {
                if (std::binary_search(v2.begin(), v2.end()-1, v1[i]))
                {
                    cr.AddHash(v1[i]);
                }
            }
        }

        std::shared_ptr<IComparisonResult> Compare(const IDocument& iSecond) const
        {
            const CompressedDocument& Second = (const CompressedDocument&)iSecond;
            CompressedComparisonResult* result = new CompressedComparisonResult(this->GetName(),Second.GetName(),mSize);
            // copy matching hashes into result
            FindMatching(this->mOdd, Second.mOdd, *result);
            FindMatching(this->mEven, Second.mEven, *result);
            return std::shared_ptr<IComparisonResult>(result);
        }

        const std::string& GetName() const
        {
            return mName;
        }

        protected:
        std::string mName;
        uint32_t mSize;
        std::vector<HASH> mOdd;
        std::vector<HASH> mEven;
    };
}

