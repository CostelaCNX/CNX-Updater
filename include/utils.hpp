#pragma once

#include <switch.h>

#include <borealis.hpp>
#include <json.hpp>
#include <regex>
#include <set>

#include "constants.hpp"

namespace util {

    typedef char NsApplicationName[0x201];
    typedef uint8_t NsApplicationIcon[0x20000];

    typedef struct
    {
        uint64_t tid;
        NsApplicationName name;
        NsApplicationIcon icon;
        brls::ListItem* listItem;
    } app;

    void clearConsole();
    bool isArchive(const std::string& path);
    void downloadArchive(const std::string& url, contentType type);
    void downloadArchive(const std::string& url, contentType type, long& status_code);
    void downloadArchiveFaster(const nlohmann::ordered_json& json, contentType type);
    void downloadArchiveFaster(const nlohmann::ordered_json& json, contentType type, long& status_code);
    void extractArchive(contentType type);
    std::string formatListItemTitle(const std::string& str, size_t maxScore = 140);
    std::string formatApplicationId(u64 ApplicationId);
    void shutDown(bool reboot = false);
    void rebootToPayload(const std::string& path);
    void showDialogBoxInfo(const std::string& text);
    int showDialogBoxBlocking(const std::string& text, const std::string& opt);
    int showDialogBoxBlocking(const std::string& text, const std::string& opt1, const std::string& opt2);
    std::string getLatestTag();
    std::string downloadFileToString(const std::string& url);
    void saveToFile(const std::string& text, const std::string& path);
    std::string readFile(const std::string& path);
    bool isErista();
    void removeSysmodulesFlags(const std::string& directory);
    std::string lowerCase(const std::string& str);
    std::string upperCase(const std::string& str);
    std::string getErrorMessage(long status_code);
    bool isApplet();
    std::string getContentsPath();
    bool getBoolValue(const nlohmann::json& jsonFile, const std::string& key);
    const nlohmann::ordered_json getValueFromKey(const nlohmann::ordered_json& jsonFile, const std::string& key);

/*
MY METHODS
*/

    void writeLog(std::string line);
    std::string getGMPackVersion();
    void doDelete(std::vector<std::string> folders);
    bool wasMOTDAlreadyDisplayed();
    std::string getMOTD(bool& bAlwaysShow);
    void createForwarderConfig();
    std::string readConfFile(const std::string& fileName, const std::string& section);
    void cleanFiles();
    void createStarFile();
    bool getLatestCFWPack(std::string& url, std::string& packName, std::string& packURL, int& packSize, std::string& packBody);
    std::string getNANDType(const std::string& NAND);
    std::string& rtrim(std::string& s);
    std::string& ltrim(std::string& s);
    std::string& trim(std::string& s);
    void removeFileWildCardFromDirectory(const std::string& directory, const std::string& fileWildCard);
    bool getGithubJSONBody(std::string url, std::string& packBody);
    void createCleanInstallFile();
    bool deleteThemeFolders();
}  // namespace util