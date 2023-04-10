// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
#pragma once

#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "icomparisonresult.h"
#include "hasher.h"

namespace FingerPrintOTron
{
    // ComparisionResult - Holds the matching hashes
    // from the CompressedDocument::Compare() and will perform analysis of the results
    // to work out the percentage of hashes that match the two 
    // documents.
    class CompressedComparisonResult : public IComparisonResult
    {
        public:
            CompressedComparisonResult(const std::string& nameFirst,
                                       const std::string& nameSecond,
                                       size_t total)
                : mNameFirst(nameFirst),
                  mNameSecond(nameSecond),
                  mTotal(total*2)
            {
            }

            ~CompressedComparisonResult() {};

            void AddHash(HASH hash)
            {
               mHashes.push_back(hash); 
            }

            void AnalyzeResults()
            {
                mPercentageFirst = (size_t)(100.0 * ((float)mHashes.size() / (float)mTotal));
                mPercentageSecond = mPercentageFirst;
            }

            size_t GetTotal() const
            { 
                return mTotal;
            }

            size_t GetPercentageSecond() const
            {
                return mPercentageSecond;
            }

            size_t GetPercentageFirst() const
            {
                return mPercentageFirst;
            }

            void SetNameFirst(const std::string& name)
            {
                mNameFirst = name;
            }

            void SetNameSecond(const std::string& name)
            {
                mNameSecond = name;
            }

            const std::string& GetNameFirst() const
            {
                return mNameFirst;
            }

            const std::string& GetNameSecond() const
            {
                return mNameSecond;
            }

        protected:
            std::vector<HASH> mHashes;
            size_t mPercentageSecond;
            size_t mPercentageFirst;
            std::string mNameFirst;
            std::string mNameSecond;
            size_t mTotal;
    };
}

