#pragma once

#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "hasher.h"

namespace FingerPrintOTron
{
    // ComparisionResult - Holds the matching sequences of hashes
    // from the Document::Compare() and will perform analysis of the results
    // to work out the percentage of hashes that match the two 
    // documents. Also stores the lengths of the longest and shortest
    // matching sequences.
    class IComparisonResult
    {
        public:
            virtual ~IComparisonResult() {};
            virtual void AnalyzeResults() = 0;
            virtual size_t GetTotal() const = 0;
            size_t GetPercentage() const { return std::max(GetPercentageSecond(),GetPercentageFirst()); } 
            virtual size_t GetPercentageSecond() const = 0;
            virtual size_t GetPercentageFirst() const = 0;
            virtual void SetNameFirst(const std::string& name) = 0;
            virtual void SetNameSecond(const std::string& name) = 0;
            virtual const std::string& GetNameFirst() const = 0;
            virtual const std::string& GetNameSecond() const = 0;
    };
}

