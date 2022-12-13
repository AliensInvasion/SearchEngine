#pragma once
#include "InvertedIndex.h"

const int defaultResponsesLimit{5};
typedef std::map<size_t, std::vector<std::string>> FrequencyList;

class SearchServer {
public:
    explicit SearchServer(InvertedIndex &invertedIndex) : idx(invertedIndex){};
    SearchServer() = default;
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queriesInput,
                                                   const int &responsesLimit = defaultResponsesLimit) const;
private:
    std::vector<RelativeIndex> getRequestIndex(const std::multimap<size_t, size_t> &docsRelevance,
                                               const int &responsesLimit,
                                               const size_t &maxAbsWordRelevance) const;

    std::multimap<size_t, size_t> getDocsRelevance(const std::set<size_t> &foundDocIds,
                                                   const std::set<std::string> &allQueryWords,
                                                   size_t &maxAbsWordRelevance) const;

    std::set<size_t> getDocIds(std::map<size_t, std::vector<std::string>> &wordsFrequencyList) const;

    FrequencyList getQueryFrequency(std::set<std::string> &queryWords) const;

    InvertedIndex idx;
};

