#ifndef CORE_SEARCHBYIMAGE_H
#define CORE_SEARCHBYIMAGE_H

#include <string>
#include <atomic>
#include <memory>
#include <functional>

#include "Core/Utils/CoreTypes.h"
#include "Network/INetworkClient.h"
#include "Core/Upload/UploadEngine.h"
#include "SearchByImageTask.h"

class INetworkClient;
class UploadManager;

class SearchByImage  {
    public:
        enum SearchEngine { seGoogle, seYandex};
        static std::unique_ptr<SearchByImageTask> createSearchEngine(std::shared_ptr<INetworkClientFactory> networkClientFactory, 
            UploadManager* uploadManager, SearchEngine se, const ServerProfile& temporaryServer, const std::string& fileName);
        static std::string getSearchEngineDisplayName(SearchEngine se);
        static std::string searchEngineTypeToString(SearchEngine se);
        static SearchEngine searchEngineTypeFromString(const std::string& name); 
protected:
    SearchByImage();
};

#endif