#pragma once

#include <vector>
#include <map>
#include <memory>
// Public Domain kick in it to ya

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

        void Compare(const Document& Second, ComparisonResult& result) const
        {
            result.SetNameFirst(this->GetName());
            result.SetNameSecond(Second.GetName());
            result.SetHashCountSecond(Second.mHashes.size());
            result.SetHashCountFirst(mHashes.size());

            std::vector<HASH>::const_iterator hashItSecond = Second.mHashes.begin();

            for (; hashItSecond != Second.mHashes.end(); ++hashItSecond)
            {
                std::shared_ptr<POSITIONS> p = Find(*hashItSecond);
                if (p)
                {
                    size_t posSecond = std::distance(Second.mHashes.begin(), hashItSecond);
                    // this is all the positions that the hash occurs
                    for (uint16_t posFirst : *p)
                    {
                        uint16_t pos2 = posFirst;
                        std::vector<HASH>::const_iterator hashFwdSecond = hashItSecond;
                        int count = 0;
                        std::shared_ptr<std::vector<HASH> > hashSequence(new std::vector<HASH>()); 
                        while (pos2 < mHashes.size() &&
                                (mHashes[pos2] == *hashFwdSecond) && 
                                (hashFwdSecond != Second.mHashes.end()))
                        {
                            hashSequence->push_back(mHashes[pos2]);
                            ++count;
                            ++pos2;
                            ++hashFwdSecond;
                        }
                        result.AddHash(posFirst,hashSequence);
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

