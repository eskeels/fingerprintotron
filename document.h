// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
#pragma once

#include <vector>
#include <map>
#include <memory>

#include "idocument.h"
#include "hasher.h"
#include "comparisonresult.h"

namespace FingerPrintOTron
{
    typedef std::vector<uint32_t> POSITIONS;
    typedef std::map<HASH,std::shared_ptr<POSITIONS> > HashIndex;
 
    // Document - Holds a map of HASH to position. The Hashes are stored in the
    // order they were calculated. Its not the position in the orginal document,
    // AddHash() simply increments a counter as they are added to preserve the
    // order. Document can compare to another document and the result goes into
    // ComparisonResult using the Compare method. Compare iterates through the
    // hashes of the document to compare against and finds them in this document.
    // If a hash is found then it checks forward for a sequence and stores that in
    // a vector which is added to the ComparisionResult.
    class Document : public IDocument
    {
       public:
        Document(const std::string& name)
            : mName(name)
        {
        }

        void AddHash(HASH hash)
        {
            mHashes.push_back(hash);
            uint32_t pos = mHashes.size()-1;
            HashIndex::iterator it = mHashIndex.find(hash);
            if (it == mHashIndex.end())
            {
                std::shared_ptr<POSITIONS> p(new POSITIONS);
                p->push_back(pos);
                mHashIndex.insert(std::make_pair(hash,p));
            }
            else
            {
                it->second->push_back(pos);
            }
        }
   
        void AddHash(std::vector<HASH> hashes)
        {
            for(auto h : hashes)
            {
                AddHash(h);
            }
        }

        std::shared_ptr<IComparisonResult> Compare(const IDocument& iSecond) const
        {
            const Document& Second = (const Document&)iSecond;
            ComparisonResult* result = new ComparisonResult(this->GetName(),
                                                            Second.GetName(),
                                                            mHashes.size(),
                                                            Second.mHashes.size());

            std::vector<HASH>::const_iterator hashItSecond = Second.mHashes.begin();

            for (; hashItSecond != Second.mHashes.end(); ++hashItSecond)
            {
                std::shared_ptr<POSITIONS> p = Find(*hashItSecond);
                if (p)
                {
                    // this is all the positions that the hash occurs
                    for (uint32_t posFirst : *p)
                    {
                        uint32_t pos2 = posFirst;
                        std::vector<HASH>::const_iterator hashFwdSecond = hashItSecond;
                        std::shared_ptr<std::vector<HASH> > hashSequence(new std::vector<HASH>()); 
                        while (pos2 < mHashes.size() &&
                                (mHashes[pos2] == *hashFwdSecond) && 
                                (hashFwdSecond != Second.mHashes.end()))
                        {
                            hashSequence->push_back(mHashes[pos2]);
                            ++pos2;
                            ++hashFwdSecond;
                        }
                        result->AddHash(posFirst,hashSequence);
                    }
                }
            }
            result->RemoveOverlaps();
            return std::shared_ptr<IComparisonResult>(result);
        }
        
        std::shared_ptr<POSITIONS> Find(HASH hash) const
        {
            HashIndex::const_iterator it = mHashIndex.find(hash);
            if (it != mHashIndex.end())
            {
                return it->second;
            }
            return NULL;
        }

        const std::string& GetName() const
        {
            return mName;
        }

        protected:
        std::string mName;
        std::vector<HASH> mHashes;
        HashIndex mHashIndex;
    };
}

