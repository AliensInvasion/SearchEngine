#pragma once
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <thread>
#include <mutex>
#include "RelativeIndex.h"


struct Entry
{
    size_t docId, count;
    bool operator ==(const Entry& other) const {
        return (docId == other.docId &&
                count == other.count);
    }
};

class InvertedIndex
{
public:

    InvertedIndex() = default;
    void UpdateDocumentBase(const std::vector<std::string> &inputDocs);
    void indexOneDocument(const std::string &document);
    void doIndexation(const std::vector<std::string> &inRequests);
    std::vector<Entry> getWordCount(const std::string& word) const;
    std::vector<Entry> getWordFrequency(const std::string &inWord) const;

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freqDictionary;
};

