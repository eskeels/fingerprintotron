#pragma once

#include <vector>
#include <map>
#include <memory>

#include "hasher.h"
#include "comparisonresult.h"

namespace FingerPrintOTron
{
    class Document
    {
        typedef std::vector<uint16_t> POSITIONS;
        typedef std::map<HASH,std::shared_ptr<POSITIONS> > HashIndex;
        public:
        Document(const std::string& name)
            : mName(name)
        {
        }

        void AddHash(HASH hash)
        {
            mHashes.push_back(hash);
            uint16_t pos = mHashes.size()-1;
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

        void Compare(const Document& RHS, ComparisonResult& result) const
        {
            result.SetHashCountRHS(RHS.mHashes.size());
            result.SetHashCountLHS(mHashes.size());

            std::vector<HASH>::const_iterator hashItRHS = RHS.mHashes.begin();

            for (; hashItRHS != RHS.mHashes.end(); ++hashItRHS)
            {
                std::shared_ptr<POSITIONS> p = Find(*hashItRHS);
                if (p)
                {
                    size_t posRHS = std::distance(RHS.mHashes.begin(), hashItRHS);
                    // this is all the positions that the hash occurs
                    for (uint16_t posLHS : *p)
                    {
                        uint16_t pos2 = posLHS;
                        std::vector<HASH>::const_iterator hashFwdRHS = hashItRHS;
                        int count = 0;
                        std::shared_ptr<std::vector<HASH> > hashSequence(new std::vector<HASH>()); 
                        while (pos2 < mHashes.size() &&
                                (mHashes[pos2] == *hashFwdRHS) && 
                                (hashFwdRHS != RHS.mHashes.end()))
                        {
                            hashSequence->push_back(mHashes[pos2]);
                            ++count;
                            ++pos2;
                            ++hashFwdRHS;
                        }
                        result.AddHash(posLHS,hashSequence);
                    }
                }
            }
            result.RemoveOverlaps();
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

        protected:
        std::string mName;
        std::vector<HASH> mHashes;
        HashIndex mHashIndex;
    };
}

