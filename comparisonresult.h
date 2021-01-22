// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.
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
    class ComparisonResult
    {
        public:
            ComparisonResult()
                : mHashCountSecond(0),
                  mHashCountFirst(0),
                  mMax(0),
                  mMin(0),
                  mTotal(0),
                  mPercentageSecond(0),
                  mPercentageFirst(0),
                  mNameFirst(),
                  mNameSecond()
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

            // Remove overlapping sequences of hashes
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
                           continue;
                        }

                        if (Within(it,next))
                        {
                            // it within nexst
                            next->second.reset();
                        }
                        else if (Within(next,it))
                        {
                           //next within it
                           it->second.reset();
                           break;
                        }
                    } 
                }
                // Overlaps were set to NULL so now
                // remove them
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

            size_t CalculatePercentage(float a, float b) const
            {
                return static_cast<size_t>(100.0 * (a / b) );
            }

            void CalculatePercentages(size_t totalSignatures, size_t& percentageSecond, size_t& percentageFirst) const
            {
                float fTotalSignatures = totalSignatures;
                float fHashCountSecond = mHashCountSecond;
                float fHashCountFirst = mHashCountFirst;

                if (fTotalSignatures > fHashCountSecond)
                {
                    percentageSecond = CalculatePercentage(fHashCountSecond, fTotalSignatures);
                }
                else
                {
                    percentageSecond = CalculatePercentage(fTotalSignatures, fHashCountSecond);
                }

                if (fTotalSignatures > fHashCountFirst)
                {
                    percentageFirst = CalculatePercentage(fHashCountFirst, fTotalSignatures);
                }
                else
                {
                    percentageFirst = CalculatePercentage(fTotalSignatures, fHashCountFirst);
                }
            }

            // Analyze the hash sequences to work out the percentages of matches
            // for the two documents. These are stored in mPercentageFirst and mPercentageSecond.
            // It also stores the length of the longest matching sequence and shortest matching
            // sequence. These are stored in mMax and mMin.
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
                    CalculatePercentages(mTotal, mPercentageSecond, mPercentageFirst);
                }
                else
                {
                    mMin = mMax = mTotal = mPercentageSecond = mPercentageFirst = 0;
                }

            }
            // Gets the size of the longest sequence of hashes
            size_t GetMax() const { return mMax; }
            // Gets the size of the shortest sequence of hashes
            size_t GetMin() const { return mMin; }
            size_t GetTotal() const { return mTotal; }
            size_t GetPercentage() const { return std::max(GetPercentageSecond(),GetPercentageFirst()); } 
            size_t GetPercentageSecond() const { return mPercentageSecond; }
            size_t GetPercentageFirst() const { return mPercentageFirst; }

            void SetHashCountSecond(size_t c)
            {
                mHashCountSecond = c;
            }

            void SetHashCountFirst(size_t c)
            {
                mHashCountFirst = c;
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
            std::multimap<size_t,std::shared_ptr<std::vector<HASH> > > mHashMap;
            size_t mHashCountSecond;
            size_t mHashCountFirst;
            size_t mMax;
            size_t mMin;
            size_t mTotal;
            size_t mPercentageSecond;
            size_t mPercentageFirst;
            std::string mNameFirst;
            std::string mNameSecond;
    };
}

