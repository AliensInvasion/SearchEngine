#include "InvertedIndex.h"

std::mutex InvertedIndex::mutex;

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string> &inputDocs) {
    docs.clear();
    for (auto &doc : inputDocs) {
        docs.emplace_back(doc);
    }
    doIndexation(docs);
}

void InvertedIndex::indexOneDocument(const std::string &document)
{
    std::string word{};
    std::stringstream docStream(document);
    while (std::getline(docStream, word, ' '))
    {
        auto it = freqDictionary.find(word);
        if (it == freqDictionary.end())
        {
            auto entries = getWordCount(word);
            std::lock_guard lock(mutex);
            freqDictionary[word] = entries;
        }
    }
}

void InvertedIndex::doIndexation(const std::vector<std::string> &documents)
{
    if (documents.empty()) return;
    unsigned long const documentsSize = documents.size();
    unsigned long const minDocumentsPerThread = 8;
    unsigned long const maxThreads = (documentsSize+minDocumentsPerThread-1)/minDocumentsPerThread;
    unsigned long const hardwareThreads = std::thread::hardware_concurrency();
    unsigned long const numThreads = std::min(hardwareThreads!=0?hardwareThreads:2,maxThreads);
    unsigned long const blockSize = documentsSize/numThreads;

    std::thread threads[numThreads-1];
    auto documentsStart = documents.begin();
    for (size_t i = 0; i < numThreads-1; ++i) {
        auto documentsEnd = documentsStart;
        std::advance(documentsEnd, blockSize);
        threads[i] = std::thread([&documents, &documentsStart, &documentsEnd, this]()
                {
                    for (auto it = documentsStart; it != documentsEnd; ++it) {
                        this->indexOneDocument(*it);
                    }
                });
        documentsStart = documentsEnd;
    }

    for (auto it = documentsStart; it != documents.end(); ++it) {
        indexOneDocument(*it);
    }

    for (auto &thread : threads) {
        thread.join();
    }

}

std::vector<Entry> InvertedIndex::getWordCount(const std::string &inWord) const
{
    std::vector<Entry> entries{};

        size_t docsSize = docs.size();

    for (size_t i = 0; i < docsSize; ++i) {
        Entry entry{i, 0};
        std::string word{};
        std::stringstream docStream(docs[i]);
        while (std::getline(docStream, word, ' '))
        {
            if (word == inWord)
                entry.count+=1;
        }
        if (entry.count > 0)
            entries.emplace_back(entry);
    }
    return entries;
}

std::vector<Entry> InvertedIndex::getWordFrequency(const std::string &inWord) const
{

    auto it = freqDictionary.find(inWord);
    if (it == freqDictionary.end()) {
        return {};
    }
    else
    {
        return it->second;
    }

}
