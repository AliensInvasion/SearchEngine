#include "SearchEngine.h"

std::vector<RelativeIndex> SearchServer::getRequestIndex(const std::multimap<size_t, size_t> &docsRelevance,
                                           const int &responsesLimit,
                                           const size_t &maxAbsWordRelevance) const
{
    size_t responsesCount{0};
    std::vector<RelativeIndex> requestIndex{};
    for (auto it = docsRelevance.rbegin(); it != docsRelevance.rend(); ++it) {
        if (responsesLimit > 0 && ++responsesCount > responsesLimit) break;
        auto currentRelativeRelevance = ((float)it->first/(float)maxAbsWordRelevance);
        RelativeIndex currentRelativeIndex{it->second, currentRelativeRelevance};
        requestIndex.emplace_back(currentRelativeIndex);
    }
    return requestIndex;
};

std::multimap<size_t, size_t> SearchServer::getDocsRelevance(const std::set<size_t> &foundDocIds,
                                                             const std::set<std::string> &allQueryWords,
                                                             size_t &maxAbsWordRelevance) const
{
    std::multimap<size_t, size_t> docsRelevance{};
    for (auto it = foundDocIds.rbegin(); it != foundDocIds.rend(); ++it) {
        size_t absRequestRelevance{0};
        for (const auto &queryWord: allQueryWords) {
            auto currentWordFrequency =
                    idx.getWordFrequency(const_cast<std::string &>(queryWord));
            for (const auto &inDocumentFrequency: currentWordFrequency) {
                if (inDocumentFrequency.docId == *it)
                    absRequestRelevance += inDocumentFrequency.count;
            }
            if (absRequestRelevance > maxAbsWordRelevance)
                maxAbsWordRelevance = absRequestRelevance;
        }
        docsRelevance.emplace(absRequestRelevance, *it);
    }
    return docsRelevance;
}

std::map<size_t, std::vector<std::string>> SearchServer::getQueryFrequency(std::set<std::string> &queryWords) const
{
    std::map<size_t, std::vector<std::string>> wordsFrequencyList{};
    for (const auto &queryWord : queryWords)
    {
        size_t wordCount{0};
        auto currentWordFrequency =
                idx.getWordFrequency(queryWord);
        for (const auto &documentFrequency : currentWordFrequency) {
            wordCount+=documentFrequency.count;
        }
        if(wordCount>0)
            wordsFrequencyList[wordCount].emplace_back(queryWord);
    }
    return wordsFrequencyList;
}

std::set<size_t> SearchServer::getDocIds(std::map<size_t, std::vector<std::string>> &queryFrequency) const
{
    std::set<size_t> foundDocIds{};
    for (const auto &currentFrequencyWords : queryFrequency) {
        for (const auto &currentFrequencyWord : currentFrequencyWords.second) {
            auto currentWordFrequency =
                    idx.getWordFrequency(currentFrequencyWord);
            for (const auto &documentFrequency : currentWordFrequency) {
                foundDocIds.emplace(documentFrequency.docId);
            }
        }
    }
    return foundDocIds;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queriesInput,
                                                             const int &responsesLimit) const {
    std::vector<std::vector<RelativeIndex>> result{};

    for (auto &query : queriesInput) {

        size_t maxAbsWordRelevance{0};
        std::stringstream queryStream(query);
        std::set<std::string> allQueryWords{};

        //Get unique words from query.
        {
            std::string queryWord{};
            while (std::getline(queryStream, queryWord, ' '))
                allQueryWords.emplace(queryWord);
        }

        //Get query words sorted by frequency.
        auto queryFrequency(getQueryFrequency(allQueryWords));
        if (queryFrequency.empty()) {
            result.emplace_back(std::vector<RelativeIndex>{});
            continue;
        }

        //Get id of all documents that contains words from query.
        auto foundDocIds(getDocIds(queryFrequency));
        if (foundDocIds.empty()) {
            result.emplace_back(std::vector<RelativeIndex>{});
            continue;
        }

        //For all found documents get request relevance.
        auto docsRelevance(getDocsRelevance(foundDocIds, allQueryWords, maxAbsWordRelevance));

        //Put current request frequency to result.
        auto requestIndex(getRequestIndex(docsRelevance, responsesLimit, maxAbsWordRelevance));
        result.emplace_back(requestIndex);
    }
    return result;
}


