#include "ConverterJSON.h"
#include "SearchEngine.h"

int main() {

    ConverterJSON converter;
    std::vector<std::string> documents, requests;

    try {
        converter.loadConfig("config.json");
    }
    catch (const ConfigFileMissingException &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const ConfigFileIsEmptyException &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const VersionIsNotCorrectException &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    documents = converter.getTextDocuments();
    requests = converter.getRequests();

    if (documents.empty()) {
        std::cerr << "No documents provided!" << std::endl;
        return EXIT_FAILURE;
    }

    if (requests.empty()) {
        std::cerr << "No requests!" << std::endl;
        return EXIT_FAILURE;
    }


    InvertedIndex idx;
    idx.UpdateDocumentBase(documents);
    SearchServer srv(idx);

    auto responsesLimit = converter.GetResponsesLimit();
    std::vector<std::vector<RelativeIndex>> result(srv.search(requests, responsesLimit));

    converter.makeAnswer(result);
}

