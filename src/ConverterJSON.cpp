#include "ConverterJSON.h"

bool ConverterJSON::getJSON(json &targetJSON, const std::string &pathToJSON) const
{
    std::ifstream JSONFile(pathToJSON);
    if (!JSONFile.is_open()) return false;
    char buffer[10];
    std::string string{};
    while (!JSONFile.eof())
    {
        JSONFile.read(buffer, sizeof(buffer));
        for (auto i = 0; i < JSONFile.gcount(); ++i) {
            string+=buffer[i];
        }
    }
    JSONFile.close();
    targetJSON = json::parse(string);
    return true;
}

void ConverterJSON::loadConfig(const std::string &path)
{
    if(!getJSON(config, path))
        throw ConfigFileMissingException();
    if (!config.contains("config"))
        throw ConfigFileIsEmptyException();
    if (config["config"]["version"]!=VERSION)
        throw VersionIsNotCorrectException();
}

std::vector<std::string> ConverterJSON::getTextDocuments() const
{

    if (config.empty()) {
        std::cerr << "Error! Config is not loaded!" << std::endl;
        return {};
    }

    auto numberOfFiles = config["files"].size();
    std::vector<std::string> textDocuments{};

    for(size_t filesCount = 0; filesCount < numberOfFiles; ++filesCount)
    {
        std::string document{};
        std::ifstream file(config["files"][filesCount]);
        if (!file.is_open()) {
            std::cerr << "Can't find: " << config["files"][filesCount] << std::endl;
            textDocuments.emplace_back(document);
            continue;
        }
        for (auto wordsCount{0}; !file.eof() && wordsCount < maxWordsInDoc; ++wordsCount) {
            std::string currentWord;
            file >> currentWord;
            if(currentWord.length() > maxCharsInWord)
                continue;
            document+=currentWord;
            if (!file.eof())
                document+=" ";
        }
        textDocuments.emplace_back(document);
    }
    return textDocuments;
}

std::vector<std::string> ConverterJSON::getRequests() const
{
    json requests{};
    std::vector<std::string> requestsVector{};
    if (!getJSON(requests, "requests.json")) {
        std::cerr << "No requests.json found!" << std::endl;
        return {};
    }

    auto numberOfRequests = requests["requests"].size();
    for (auto reqCount = 0; reqCount < numberOfRequests && reqCount < maxRequests; ++reqCount) {
        std::stringstream requestStream{std::string(requests["requests"][reqCount])};
        std::string request{};
        for (auto wordsCount{0}; !requestStream.eof() && wordsCount < maxWordsInRequest; ++wordsCount) {
            std::string word{};
            requestStream >> word;
            request+=word;
            request+=' ';
        }
        requestsVector.emplace_back(request);
    }
    return requestsVector;
}


int ConverterJSON::GetResponsesLimit() const
{
    return config["config"]["max_responses"];
}


void ConverterJSON::makeAnswer(const std::vector<std::vector<RelativeIndex>> &result) const
{
    json answers{};
    auto count{0};
    for (auto &requestDocumentsIndex : result) {
        ++count;
        std::stringstream makeNameStream{};
        makeNameStream << std::setw(3) << std::setfill('0') << count;

        std::string reqName{"request"};
        reqName += makeNameStream.str();

        if (requestDocumentsIndex.empty()) {
            answers["answers"][reqName]["result"] = false;
            continue;
        }

        answersVector ans{};
        for (auto &documentIndex : requestDocumentsIndex) {
            ans.emplace_back(std::make_pair("docid", documentIndex.docId),
                              std::make_pair("rank", documentIndex.rank));
        }

        if (ans.size() == 1)
            answers["answers"][reqName] = ans[0];
        else
            answers["answers"][reqName]["relevance"] = ans;
        answers["answers"][reqName]["result"] = true;
    }

    auto answerString = answers.dump(4);
    std::ofstream answerJSON("answer.json");
    answerJSON.write(answerString.c_str(), (long long)answerString.size());
    answerJSON.close();
}
