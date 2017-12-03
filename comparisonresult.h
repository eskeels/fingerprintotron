#pragma once

#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "hasher.h"

namespace FingerPrintOTron
{
    class ComparisonResult
    {
        public:
            ComparisonResult()
                : mHashCountRHS(0),
                  mHashCountLHS(0),
                  mMax(0),
                  mMin(0),
                  mTotal(0),
                  mPercentageRHS(0),
                  mPercentageLHS(0)
            {
            }

            void AddHash(size_t position, std::shared_ptr<std::vector<HASH> > hashes)
            {
                bool insert = true;
                auto i = mHashMap.find(position);
                if (i != mHashMap.end())
                {
                    if (i->second->size() >= hashes->size())
                    {
                        // existing hash at that position
                        // is equal OR longer than the one trying to insert
                        insert = false;
                    }
                    else// if (i->second->size() < hashes->size())
                    {
                        mHashMap.erase(i);
                    }
                }

                if (insert)
                {
                    mHashMap.insert(std::make_pair(position,hashes));
                }
            }

            void CalculateStartEnd(std::map<size_t,std::shared_ptr<std::vector<HASH> > >::iterator& it,
                                    size_t& start,
                                    size_t& end) const
            {
                start = it->first;
                end = it->first + it->second->size()-1;
            }

            void RemoveOverlaps()
            {
                auto it = mHashMap.begin();
                for ( ; it != mHashMap.end() ; ++it)
                {
                    if (it->second == NULL)
                    {
                        continue;
                    }

                    auto next = it;
                    ++next;
                    for ( ; next != mHashMap.end(); ++next)
                    {
                        if (next->second == NULL)
                        {
                           exit(1);
                           //continue;
                        }

                        if (Within(it,next))
                        {
                            // it within nexst
                            next->second.reset();
                        }
                        else if (Within(next,it))
                        {
                           exit(1);
                           //next within it
                           //it->second.reset();
                           //break;
                        }
                    } 
                }
                RemoveNULLs();
            }

            void RemoveNULLs()
            {
                auto it = mHashMap.begin();
                for ( ; it != mHashMap.end() ; )
                {
                    if (it->second == NULL)
                    {
                        it = mHashMap.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            bool Within(std::map<size_t,std::shared_ptr<std::vector<HASH> > >::iterator& first,
                        std::map<size_t,std::shared_ptr<std::vector<HASH> > >::iterator& second) const
            {
                size_t start1st,end1st;
                CalculateStartEnd(first, start1st, end1st);

                size_t start2nd,end2nd;
                CalculateStartEnd(second, start2nd, end2nd);

                return Within(start1st, end1st, start2nd, end2nd);
            } 

            bool Within(size_t start1, size_t end1, size_t start2, size_t end2) const
            {
                if (start2 >= start1 && end2 <= end1)
                {
                    return true;
                }
             
                return false;
            }
            
            void Dump(std::stringstream& out, std::shared_ptr<std::vector<HASH> > seq) const
            {
                std::vector<HASH>& hashes = *seq;
                for (auto& h : hashes)
                {
                    out << h << ",";
                }
            }

            void Dump(std::stringstream& out) const
            {
                std::map<size_t,std::shared_ptr<std::vector<HASH> > >::const_iterator it;

                it = mHashMap.begin();
                for ( ; it != mHashMap.end() ; ++it)
                {
                    out << it->first << " - " << it->first + it->second->size()-1 << " ";
                    Dump(out, it->second);
                    out << std::endl;
                }
            }

            std::shared_ptr<std::vector<HASH> > GetHash(size_t pos)
            {
                auto it = mHashMap.find(pos);
                if (it!=mHashMap.end())
                {
                    return it->second;
                }
                return NULL;
            }

            bool ZeroMatch() const
            {
                return mHashMap.empty();
            }
            
            void CalculatePercentages(size_t totalSignatures, size_t& percentageRHS, size_t& percentageLHS) const
            {
                float fTotalSignatures = totalSignatures;
                percentageRHS = static_cast<size_t>(100.0 * (fTotalSignatures / float(mHashCountRHS)));
                percentageLHS = static_cast<size_t>(100.0 * (fTotalSignatures / float(mHashCountLHS)));
            }

            void AnalyzeResults() 
            {
                if (!mHashMap.empty())
                {
                    auto it = mHashMap.begin();
                    mTotal = mMax = mMin = it->second->size();
                    ++it;
                    for ( ; it != mHashMap.end() ; ++it)
                    {
                        size_t fpSz = it->second->size();
                        if (fpSz > mMax)
                        {
                            mMax = fpSz;
                        }
                        else if (fpSz < mMin)
                        {
                            mMin = fpSz;
                        }
                        mTotal += fpSz;
                    }
                    CalculatePercentages(mTotal, mPercentageRHS, mPercentageLHS);
                }
                else
                {
                    mMin = mMax = mTotal = mPercentageRHS = mPercentageLHS = 0;
                }

            }

            size_t GetMax() const { return mMax; }
            size_t GetMin() const { return mMin; }
            size_t GetTotal() const { return mTotal; }
            size_t GetPercentageRHS() const { return mPercentageRHS; }
            size_t GetPercentageLHS() const { return mPercentageLHS; }

            void SetHashCountRHS(size_t c)
            {
                mHashCountRHS = c;
            }

            void SetHashCountLHS(size_t c)
            {
                mHashCountLHS = c;
            }

        protected:
            std::multimap<size_t,std::shared_ptr<std::vector<HASH> > > mHashMap;
            size_t mHashCountRHS;
            size_t mHashCountLHS;
            size_t mMax;
            size_t mMin;
            size_t mTotal;
            size_t mPercentageRHS;
            size_t mPercentageLHS;
    };
}

