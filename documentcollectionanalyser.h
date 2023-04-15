#pragma once

#include <vector>
#include <set>
#include <string>

#include "comparisonresult.h"
#include "compressedcomparisonresult.h"

namespace FingerPrintOTron
{
    // DocumentCollectionAnalyser - Takes in a list of Document objects
    // and a threshold. Each document is compared to the other and if the
    // comparion results in a percentage match greater than the threshold
    // then those 2 documents are recorded as being similar. All similar
    // documents are stored in a set. The Dump() method will output the
    //  results to a string stream.
    class DocumentCollectionAnalyser
    {
        public:
            DocumentCollectionAnalyser(std::vector<std::shared_ptr<IDocument> >& docs, uint32_t threshold)
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

            void RecordResult(std::shared_ptr<IComparisonResult> cr)
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

                mListComparisonResults.push_back(cr);
            }

            void AnalysePair(const IDocument& first, const IDocument& second)
            {
                //std::shared_ptr<IComparisonResult> result(new CompressedComparisonResult(100));
                std::shared_ptr<IComparisonResult> result = first.Compare(second);
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
                for (auto cr : mListComparisonResults)
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
            // Vector of all the documents that should be analyzed by comparing with
            // each other
            std::vector<std::shared_ptr<IDocument> >& mDocuments;
            // This vector contains sets of similar documents
            std::vector<std::shared_ptr<std::set<std::string> > > mListFileSets;
            // This contains the results of each comparision
            std::vector<std::shared_ptr<IComparisonResult> > mListComparisonResults;
            // The percentage threshold that must be exceeded before 2 documents
            // are considered as similar
            uint32_t mThreshold;
    };
}
