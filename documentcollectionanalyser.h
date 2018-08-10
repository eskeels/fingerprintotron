#pragma once

#include <vector>
#include <set>
#include <string>

namespace FingerPrintOTron
{
    class DocumentCollectionAnalyser
    {
        public:
            DocumentCollectionAnalyser(std::vector<std::shared_ptr<Document> >& docs, uint16_t threshold)
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

            void RecordResult(ComparisonResult& cr)
            {
                std::cout << cr.GetNameFirst() << " - " << cr.GetNameSecond() << " %" << cr.GetPercentage() << std::endl;
                if (cr.GetPercentage() >= mThreshold)
                {
                    std::shared_ptr<std::set<std::string> > fileSet( FindFileSet(cr.GetNameFirst(), cr.GetNameSecond()) );
                    if (fileSet)
                    {
                        fileSet->insert( cr.GetNameFirst() );
                        fileSet->insert( cr.GetNameSecond() );
                    }
                }
            }

            void AnalysePair(const Document& first, const Document& second)
            {
                std::shared_ptr<ComparisonResult> result(new ComparisonResult);
                first.Compare(second,*result);
                result->AnalyzeResults();
                RecordResult(*result);
            }

            void Analyse()
            {
                std::cout << "Analysing documents:" << std::endl;
                for (size_t i = 0; i < mDocuments.size(); ++i)
                {
                    for (size_t j = i+1; j < mDocuments.size(); ++j)
                    {
                        AnalysePair(*mDocuments[i], *mDocuments[j]);
                    }
                }
                std::cout << std::endl;
            }

            void Dump()
            {
                if (!mListFileSets.empty())
                {
                    std::cout << "Similar documents:" << std::endl;
                    for (auto it = mListFileSets.begin() ; it != mListFileSets.end(); ++it)
                    {
                        std::set<std::string>& fileSet = *(*it);
                        for (auto fileIt = fileSet.begin(); fileIt != fileSet.end(); ++fileIt)
                        {
                            std::cout << *fileIt << " ";
                        }
                        std::cout << std::endl;
                    }
                }
                else
                {
                    std::cout << "No similar documents found." << std::endl;
                }
                std::cout << std::endl;
            }

        protected:
            std::vector<std::shared_ptr<Document> >& mDocuments;
            std::vector<std::shared_ptr<std::set<std::string> > > mListFileSets;
            uint16_t mThreshold;
    };
}
