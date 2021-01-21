// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#pragma once

#include <vector>
#include <set>
#include <string>

#include "comparisonresult.h"

namespace FingerPrintOTron
{
    class DocumentCollectionAnalyser
    {
        public:
            DocumentCollectionAnalyser(std::vector<std::shared_ptr<Document> >& docs, uint32_t threshold)
                : mDocuments(docs),
                  mThreshold(threshold)
            {
            }

            std::shared_ptr<std::set<std::string> > FindFileSet(const std::string& first, const std::string& second)
            {
                if (mListFileSets.empty())
                {
                    std::shared_ptr<std::set<std::string> > newFileSet(new std::set<std::string>());
                    mListFileSets.push_back(newFileSet);

                    return newFileSet;
                }
                else
                {
                    for (auto it = mListFileSets.begin() ; it != mListFileSets.end(); ++it)
                    {
                        std::set<std::string>& fileSet = *(*it);

                        if ( fileSet.end() != fileSet.find(first) || fileSet.end() != fileSet.find(second) )
                        {
                            return *it;
                        }
                        else
                        {
                            std::shared_ptr<std::set<std::string> > newFileSet(new std::set<std::string>());
                            mListFileSets.push_back(newFileSet);
                            return newFileSet;
                        }
                    }
                }

                return NULL;
            }

            void RecordResult(std::shared_ptr<ComparisonResult> cr)
            {
                if (cr->GetPercentage() >= mThreshold)
                {
                    std::shared_ptr<std::set<std::string> > fileSet( FindFileSet(cr->GetNameFirst(), cr->GetNameSecond()) );
                    if (fileSet)
                    {
                        fileSet->insert( cr->GetNameFirst() );
                        fileSet->insert( cr->GetNameSecond() );
                    }
                }

                mListComparisionResults.push_back(cr);
            }

            void AnalysePair(const Document& first, const Document& second)
            {
                std::shared_ptr<ComparisonResult> result(new ComparisonResult);
                first.Compare(second,*result);
                result->AnalyzeResults();
                RecordResult(result);
            }

            void Analyse()
            {
                for (size_t i = 0; i < mDocuments.size(); ++i)
                {
                    for (size_t j = i+1; j < mDocuments.size(); ++j)
                    {
                        AnalysePair(*mDocuments[i], *mDocuments[j]);
                    }
                }
            }

            void Dump(std::stringstream& out)
            {
                for (auto cr : mListComparisionResults)
                {
                    out << cr->GetNameFirst() << " - " << cr->GetNameSecond() << " %" << cr->GetPercentage() << std::endl;
                }
                out << std::endl;
                if (!mListFileSets.empty())
                {
                    out << "Similar documents:" << std::endl;
                    for (auto it = mListFileSets.begin() ; it != mListFileSets.end(); ++it)
                    {
                        std::set<std::string>& fileSet = *(*it);
                        for (auto fileIt = fileSet.begin(); fileIt != fileSet.end(); ++fileIt)
                        {
                            out << *fileIt << " ";
                        }
                        out << std::endl;
                    }
                }
                else
                {
                    out << "No similar documents found." << std::endl;
                }
                out << std::endl;
            }

        protected:
            std::vector<std::shared_ptr<Document> >& mDocuments;
            // This vector contains sets of similar documents
            std::vector<std::shared_ptr<std::set<std::string> > > mListFileSets;
            // This contains the results of each comparision
            std::vector<std::shared_ptr<ComparisonResult> > mListComparisionResults;
            uint32_t mThreshold;
    };
}
