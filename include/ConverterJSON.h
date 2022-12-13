#pragma once
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include "nlohmann/json.hpp"
#include "RelativeIndex.h"
#include "version.h"

using json = nlohmann::json;

const int maxRequests{1000};
const int maxWordsInRequest{10};
const int maxWordsInDoc{1000};
const int maxCharsInWord{100};

typedef std::vector<std::pair<std::pair<std::string, size_t>,
        std::pair<std::string, float>>> answersVector;

class ConfigFileMissingException : std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Config file not found!";
    }
};

class ConfigFileIsEmptyException : std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Config file is empty!";
    }
};

class VersionIsNotCorrectException : std::exception
{
public:
     const char* what() const noexcept override
    {
        return "config.json has incorrect file version";
    }
};


class ConverterJSON
{
public:
    ConverterJSON() = default;
    void loadConfig(const std::string &path);
    void makeAnswer(const std::vector<std::vector<RelativeIndex>> &result) const;
    std::vector<std::string> getTextDocuments() const;
    std::vector<std::string> getRequests() const;
    int GetResponsesLimit() const;

private:
    bool getJSON(json &targetJSON, const std::string &path) const;
    json config;
};
