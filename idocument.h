// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
#pragma once

#include <vector>
#include <map>
#include <memory>

#include "hasher.h"
#include "icomparisonresult.h"

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
    class IDocument
    {
        public:
        virtual ~IDocument() {};
        virtual void AddHash(HASH hash) = 0;
        virtual std::shared_ptr<IComparisonResult> Compare(const IDocument& iSecond) const = 0;

    };
}

