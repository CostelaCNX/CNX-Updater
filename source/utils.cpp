#include "utils.hpp"

#include <switch.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include "unistd.h"
#include "reboot_payload.h"

#include "fs.hpp"

#include "current_cfw.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "main_frame.hpp"
#include "progress_event.hpp"
#include "constants.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

namespace util {

    bool isArchive(const std::string& path)
    {
        if (std::filesystem::exists(path)) {
            std::ifstream is(path, std::ifstream::binary);
            char zip_signature[4] = {0x50, 0x4B, 0x03, 0x04};  // zip signature header PK\3\4
            char signature[4];
            is.read(signature, 4);
            if (is.good() && std::equal(std::begin(signature), std::end(signature), std::begin(zip_signature), std::end(zip_signature))) {
                return true;
            }
        }
        return false;
    }

    void downloadArchive(const std::string& url, contentType type)
    {
        long status_code;
        downloadArchive(url, type, status_code);
    }

    void downloadArchive(const std::string& url, contentType type, long& status_code)
    {
        fs::createTree(fmt::format(DOWNLOAD_PATH, BASE_FOLDER_NAME));
        switch (type) {
            case contentType::fw:
                status_code = download::downloadFile(url, fmt::format(FIRMWARE_FILENAME, BASE_FOLDER_NAME), OFF);
                break;
            case contentType::app:
                status_code = download::downloadFile(url, fmt::format(APP_FILENAME, BASE_FOLDER_NAME), OFF);
                break;
            case contentType::ams_cfw:
                status_code = download::downloadFile(url, fmt::format(AMS_FILENAME, BASE_FOLDER_NAME), OFF);
                break;
            case contentType::translations:
                status_code = download::downloadFile(url, fmt::format(TRANSLATIONS_ZIP_PATH, BASE_FOLDER_NAME), OFF);
                break;
            case contentType::modifications:
                status_code = download::downloadFile(url, fmt::format(MODIFICATIONS_ZIP_PATH, BASE_FOLDER_NAME), OFF);
                break;
            default:
                break;
        }
        ProgressEvent::instance().setStatusCode(status_code);
    }

    void downloadArchiveFaster(const nlohmann::ordered_json& json, contentType type)
    {
        long status_code;
        downloadArchiveFaster(json, type, status_code);
    }

    void downloadArchiveFaster(const nlohmann::ordered_json& json, contentType type, long& status_code)
    {
        fs::createTree(fmt::format(DOWNLOAD_PATH, BASE_FOLDER_NAME));
        switch (type) {
            case contentType::fw:
                //status_code = download::downloadFileFaster(json, fmt::format(FIRMWARE_FILENAME, BASE_FOLDER_NAME), OFF);
                break;
            case contentType::ams_cfw:
                status_code = download::downloadFileFaster(json, CFW_ROOT_PATH, OFF);
                break;
            default:
                break;
        }
        ProgressEvent::instance().setStatusCode(status_code);
    }

    void showDialogBoxInfo(const std::string& text)
    {
        brls::Dialog* dialog;
        dialog = new brls::Dialog(text);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    }

    int showDialogBoxBlocking(const std::string& text, const std::string& opt)
    {
        int result = -1;
        brls::Dialog* dialog = new brls::Dialog(text);
        brls::GenericEvent::Callback callback = [dialog, &result](brls::View* view) {
            result = 0;
            dialog->close();
        };
        dialog->addButton(opt, callback);
        dialog->setCancelable(false);
        dialog->open();
        while (result == -1) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        std::this_thread::sleep_for(std::chrono::microseconds(800000));
        return result;
    }

    int showDialogBoxBlocking(const std::string& text, const std::string& opt1, const std::string& opt2)
    {
        int result = -1;
        brls::Dialog* dialog = new brls::Dialog(text);
        brls::GenericEvent::Callback callback1 = [dialog, &result](brls::View* view) {
            result = 0;
            dialog->close();
        };
        brls::GenericEvent::Callback callback2 = [dialog, &result](brls::View* view) {
            result = 1;
            dialog->close();
        };
        dialog->addButton(opt1, callback1);
        dialog->addButton(opt2, callback2);
        dialog->setCancelable(false);
        dialog->open();
        while (result == -1) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        std::this_thread::sleep_for(std::chrono::microseconds(800000));
        return result;
    }

    void crashIfNotArchive(contentType type)
    {
        std::string filename;
        switch (type) {
            case contentType::fw:
                filename = fmt::format(FIRMWARE_FILENAME, BASE_FOLDER_NAME);
                break;
            case contentType::app:
                filename = fmt::format(APP_FILENAME, BASE_FOLDER_NAME);
                break;
            case contentType::ams_cfw:
                filename = fmt::format(AMS_FILENAME, BASE_FOLDER_NAME);
                break;
            default:
                return;
        }
        if (!isArchive(filename)) {
            brls::Application::crash("menus/utils/not_an_archive"_i18n);
        }
    }

    void extractArchive(contentType type)
    {
        chdir(ROOT_PATH);
        crashIfNotArchive(type);
        switch (type) {
            case contentType::fw:
                if (std::filesystem::exists(FIRMWARE_PATH)) std::filesystem::remove_all(FIRMWARE_PATH);
                fs::createTree(FIRMWARE_PATH);
                extract::extract(fmt::format(FIRMWARE_FILENAME, BASE_FOLDER_NAME), FIRMWARE_PATH);

                //removing firmware temporary zip file.
                std::filesystem::remove(fmt::format(FW_ZIP_PATH, BASE_FOLDER_NAME));
                break;
            case contentType::app:
                extract::extract(fmt::format(APP_FILENAME, BASE_FOLDER_NAME), fmt::format(CONFIG_PATH, BASE_FOLDER_NAME));
                fs::copyFile(ROMFS_FORWARDER, fmt::format(FORWARDER_PATH, BASE_FOLDER_NAME));

                //creting the star file
                createStarFile();

                //removing update temporary zip file.
                std::filesystem::remove(fmt::format(APP_ZIP_PATH, BASE_FOLDER_NAME));

                createForwarderConfig();
                envSetNextLoad(fmt::format(FORWARDER_PATH, BASE_FOLDER_NAME).c_str(), fmt::format("\"{}\"", fmt::format(FORWARDER_PATH, BASE_FOLDER_NAME)).c_str());
                romfsExit();
                brls::Application::quit();
                break;
            case contentType::ams_cfw: {
                std::filesystem::remove(CLEAN_INSTALL_FLAG);

                int overwriteInis = 1;
                //int overwriteInis = showDialogBoxBlocking("menus/utils/overwrite_inis"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n);

                int deleteContents = 1;
                //int deleteContents = showDialogBoxBlocking("menus/utils/delete_sysmodules_flags"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n);

                int fullClean = showDialogBoxBlocking("menus/utils/clean_install"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n);

                if (fullClean == 1)
                {
                    if (showDialogBoxBlocking("menus/utils/clean_install_confirm"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n) == 1)
                        createCleanInstallFile();
                    else
                        fullClean = 0;
                }

                if ((deleteContents == 1) && (fullClean == 0))
                {
                    removeSysmodulesFlags(AMS_CONTENTS);
                    removeFileWildCardFromDirectory(ROOT_PATH, "hekate_ctcaer_");
					
					extract::extract(fmt::format(AMS_FILENAME, BASE_FOLDER_NAME), ROOT_PATH, overwriteInis);
                }
				else
					extract::extract(fmt::format(AMS_FILENAME, BASE_FOLDER_NAME), CFW_ROOT_PATH, overwriteInis);

                //removing custom firmware temporary zip file.
                std::filesystem::remove(fmt::format(AMS_ZIP_PATH, BASE_FOLDER_NAME));
                break;
            }
            case contentType::translations:
                extract::extract(fmt::format(TRANSLATIONS_ZIP_PATH, BASE_FOLDER_NAME), AMS_CONTENTS);
                break;
            case contentType::modifications:
                extract::extract(fmt::format(MODIFICATIONS_ZIP_PATH, BASE_FOLDER_NAME), AMS_CONTENTS);
                break;

            default:
                break;
        }
        if (type == contentType::ams_cfw)
            fs::copyFiles(fmt::format(COPY_FILES_TXT, BASE_FOLDER_NAME));
    }

    std::string formatListItemTitle(const std::string& str, size_t maxScore)
    {
        size_t score = 0;
        for (size_t i = 0; i < str.length(); i++) {
            score += std::isupper(str[i]) ? 4 : 3;
            if (score > maxScore) {
                return str.substr(0, i - 1) + "\u2026";
            }
        }
        return str;
    }

    std::string formatApplicationId(u64 ApplicationId)
    {
        return fmt::format("{:016X}", ApplicationId);
    }

    void shutDown(bool reboot)
    {
        spsmInitialize();
        spsmShutdown(reboot);
    }

    void rebootToPayload(const std::string& path)
    {
        reboot_to_payload(path.c_str(), CurrentCfw::running_cfw != CFW::ams);
    }

    std::string getLatestTag()
    {
        nlohmann::ordered_json tag;
        download::getRequest(fmt::format(APP_INFO, GITHUB_USER, BASE_FOLDER_NAME), tag, {"accept: application/vnd.github.v3+json"});
        if (tag.find("tag_name") != tag.end())
            return tag["tag_name"];
        else
            return "";
    }

    std::string downloadFileToString(const std::string& url)
    {
        std::vector<uint8_t> bytes;
        download::downloadFile(url, bytes);
        std::string str(bytes.begin(), bytes.end());
        return str;
    }

    void saveToFile(const std::string& text, const std::string& path)
    {
        std::ofstream file(path);
        file << text << std::endl;
    }

    std::string readFile(const std::string& path)
    {
        std::string text = "";
        std::ifstream file(path);
        if (file.good()) {
            file >> text;
        }
        return text;
    }

    bool isErista()
    {
        SetSysProductModel model;
        setsysGetProductModel(&model);
        return (model == SetSysProductModel_Nx || model == SetSysProductModel_Copper);
    }

    void removeSysmodulesFlags(const std::string& directory)
    {
        for (const auto& e : std::filesystem::recursive_directory_iterator(directory)) {
            if (e.path().string().find("boot2.flag") != std::string::npos) {
                std::filesystem::remove(e.path());
            }
        }
    }
    
    std::string lowerCase(const std::string& str)
    {
        std::string res = str;
        std::for_each(res.begin(), res.end(), [](char& c) {
            c = std::tolower(c);
        });
        return res;
    }

    std::string upperCase(const std::string& str)
    {
        std::string res = str;
        std::for_each(res.begin(), res.end(), [](char& c) {
            c = std::toupper(c);
        });
        return res;
    }

    std::string getErrorMessage(long status_code)
    {
        std::string res;
        switch (status_code) {
            case 500:
                res = fmt::format("{0:}: Internal Server Error", status_code);
                break;
            case 503:
                res = fmt::format("{0:}: Service Temporarily Unavailable", status_code);
                break;
            default:
                res = fmt::format("error: {0:}", status_code);
                break;
        }
        return res;
    }

    bool isApplet()
    {
        AppletType at = appletGetAppletType();
        return at != AppletType_Application && at != AppletType_SystemApplication;
    }

    std::set<std::string> getExistingCheatsTids()
    {
        std::string path = getContentsPath();
        std::set<std::string> res;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::string cheatsPath = entry.path().string() + "/cheats";
            if (std::filesystem::exists(cheatsPath)) {
                res.insert(util::upperCase(cheatsPath.substr(cheatsPath.length() - 7 - 16, 16)));
            }
        }
        return res;
    }

    std::string getContentsPath()
    {
        std::string path;
        switch (CurrentCfw::running_cfw) {
            case CFW::ams:
                path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
                break;
            case CFW::rnx:
                path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
                break;
            case CFW::sxos:
                path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
                break;
        }
        return path;
    }

    bool getBoolValue(const nlohmann::json& jsonFile, const std::string& key)
    {
        /* try { return jsonFile.at(key); }
    catch (nlohmann::json::out_of_range& e) { return false; } */
        return (jsonFile.find(key) != jsonFile.end()) ? jsonFile.at(key).get<bool>() : false;
    }

    const nlohmann::ordered_json getValueFromKey(const nlohmann::ordered_json& jsonFile, const std::string& key)
    {
        return (jsonFile.find(key) != jsonFile.end()) ? jsonFile.at(key) : nlohmann::ordered_json::object();
    }

/*
MY METHODS
*/

    void writeLog(std::string line)
    {
        std::ofstream logFile;
        logFile.open(fmt::format(LOG_FILE, BASE_FOLDER_NAME), std::ofstream::out | std::ofstream::app);
        if (logFile.is_open()) {
            logFile << line << std::endl;
        }
        logFile.close();
    }

    std::string getGMPackVersion()
    {
        std::ifstream file(PACK_FILE);
        std::string line;

        if (file.is_open())
        {
            while (std::getline(file, line)) {
                if(line.find("{GMPACK", 0) != std::string::npos)
                {
                    line = util::trim(line);
                    line = line.substr(1, line.size() - 2);
                    break;
                }
                else
                    line = "";
            }
            file.close();
        }
        return line;
    }

    void doDelete(std::vector<std::string> folders)
    {
        if (!folders.empty())
        {
            ProgressEvent::instance().setTotalSteps(folders.size());
            ProgressEvent::instance().setStep(0);

            int index;
            std::string tmpFolder;
            std::string path = getContentsPath();

            for (std::string f : folders) {
                tmpFolder = f + "/";
                std::filesystem::remove_all(path + tmpFolder);
                index = tmpFolder.find_last_of("/");
                tmpFolder = tmpFolder.substr(0, index);

                while(tmpFolder.find_last_of("/") != std::string::npos)
                {
                    index = tmpFolder.find_last_of("/");
                    tmpFolder = tmpFolder.substr(0, index);

                    if (std::filesystem::exists(path + tmpFolder))
                    {
                        if (std::filesystem::is_empty(path + tmpFolder))
                            std::filesystem::remove_all(path + tmpFolder);
                    }
                }

                ProgressEvent::instance().incrementStep(1);
            }
        }
    }


    bool wasMOTDAlreadyDisplayed()
    {
        std::string hiddenFileLine;

        bool bAlwaysShow = false;

        std::string MOTDLine = getMOTD(bAlwaysShow);
        if (MOTDLine != "")
        {
            if (bAlwaysShow)
                return false;

            //getting only the first line of the MOTD
            std::string firstLine;
            size_t pos_end;

            if ((pos_end = MOTDLine.find("\n", 0)) != std::string::npos)
                firstLine = MOTDLine.substr(0, pos_end);
            else
                firstLine = MOTDLine;

            std::ifstream MOTDFile;
            MOTDFile.open(fmt::format(HIDDEN_APG_FILE, BASE_FOLDER_NAME, BASE_FOLDER_NAME));
            if (MOTDFile.is_open()) {
                getline(MOTDFile, hiddenFileLine);
            }
            MOTDFile.close();
            return (hiddenFileLine == firstLine);
        }
        else
            return true;
    }

    std::string getMOTD(bool& bAlwaysShow)
    {
        std::string text = "";
        nlohmann::ordered_json json;
        download::getRequest(NXLINKS_URL, json);
        if (json.size())
        {
            if (json.find(fmt::format(MOTD_KEY, util::upperCase(BASE_FOLDER_NAME))) != json.end())
            {
                bool enabled = json[fmt::format(MOTD_KEY, util::upperCase(BASE_FOLDER_NAME))]["enabled"];
                if (enabled)
                {
                    text = json[fmt::format(MOTD_KEY, util::upperCase(BASE_FOLDER_NAME))]["message"];
                    bAlwaysShow = json[fmt::format(MOTD_KEY, util::upperCase(BASE_FOLDER_NAME))]["always_show"];
                }
            }
        }

        return text;
    }

    void createForwarderConfig()
    {
        std::ofstream file;
        file.open(fmt::format(FORWARDER_CONF, BASE_FOLDER_NAME), std::ofstream::out | std::ofstream::trunc);
        if (file.is_open()) {
            file << fmt::format("PATH=/switch/{}-updater/", BASE_FOLDER_NAME) << std::endl;
            file << fmt::format("FULL_PATH=/switch/{}-updater/{}-updater.nro", BASE_FOLDER_NAME, BASE_FOLDER_NAME) << std::endl;
            file << fmt::format("CONFIG_PATH=/config/{}-updater/switch/{}-updater/{}-updater.nro", BASE_FOLDER_NAME, BASE_FOLDER_NAME, BASE_FOLDER_NAME) << std::endl;
            file << fmt::format("PREFIX=/switch/{}-updater/{}-updater-v", BASE_FOLDER_NAME, BASE_FOLDER_NAME) << std::endl;
            file << fmt::format("FORWARDER_PATH=/config/{}-updater/app-forwarder.nro", BASE_FOLDER_NAME) << std::endl;
            file << fmt::format("CONFIG_SWITCH=/config/{}-updater/switch/", BASE_FOLDER_NAME) << std::endl;
            file << fmt::format("HIDDEN_FILE=/config/{}-updater/.{}-updater", BASE_FOLDER_NAME, BASE_FOLDER_NAME) << std::endl;
        }
        file.close();
    }

    std::string readConfFile(const std::string& fileName, const std::string& section)
    {
        std::ifstream file(fileName.c_str());
        std::string line;

        if (file.is_open())
        {
            size_t pos_char;
            while (std::getline(file, line)) {
                if ((pos_char = line.find(section + "=", 0)) != std::string::npos)
                {
                    line = line.substr((pos_char + section.size() + 1), line.size() - (section.size() + 1));
                    break;
                }
                else
                    line = "";
            }
            file.close();
        }
        return line;
    }

    void cleanFiles()
    {
        std::filesystem::remove(fmt::format(AMS_ZIP_PATH, BASE_FOLDER_NAME));
        std::filesystem::remove(fmt::format(APP_ZIP_PATH, BASE_FOLDER_NAME));
        std::filesystem::remove(fmt::format(FW_ZIP_PATH, BASE_FOLDER_NAME));
        std::filesystem::remove(fmt::format(TRANSLATIONS_ZIP_PATH, BASE_FOLDER_NAME));
        std::filesystem::remove(fmt::format(MODIFICATIONS_ZIP_PATH, BASE_FOLDER_NAME));
        std::filesystem::remove(fmt::format(LOG_FILE, BASE_FOLDER_NAME));
        std::filesystem::remove(fmt::format(FORWARDER_CONF, BASE_FOLDER_NAME));
        std::filesystem::remove(CLEAN_INSTALL_FLAG);
        fs::removeDir(CFW_ROOT_PATH);
        fs::removeDir(fmt::format(AMS_DIRECTORY_PATH, BASE_FOLDER_NAME));
        fs::removeDir(fmt::format(SEPT_DIRECTORY_PATH, BASE_FOLDER_NAME));
        fs::removeDir(FW_DIRECTORY_PATH);
    }

    void createStarFile()
    {
        std::ofstream starFile(fmt::format(APG_STAR_FILE, BASE_FOLDER_NAME, BASE_FOLDER_NAME));
    }

    bool getLatestCFWPack(std::string& url, std::string& packName, std::string& packURL, int& packSize, std::string& packBody)
    {
        nlohmann::ordered_json json;
        download::getRequest(url, json, {"accept: application/vnd.github.v3+json"});
        if (json.find("name") != json.end())
            packName = json["name"];
        else
            return false;
        
        if (json.find("body") != json.end())
            packBody = json["body"];
        else
            return false;

        json = getValueFromKey(json, "assets");
        if (!json.empty())
        {
            packURL = json[0]["browser_download_url"];
            packSize = json[0]["size"];
        }
        else
            return false;

        return true;
    }

    std::string getNANDType(const std::string& NAND)
    {
        std::string type = "";
        if (NAND.find("emu", 0) != std::string::npos)
        {
            if (std::filesystem::exists(AMS_EMUNAND_FILE))
            {
                std::ifstream fEmuFile(AMS_EMUNAND_FILE);
                std::string line;

                if (fEmuFile.is_open())
                {
                    while (std::getline(fEmuFile, line)) {
                        if(line.find("nintendo_path=emuMMC/SD", 0) != std::string::npos)
                        {
                            type = " em arquivos)";
                            break;
                        }
                        else
                            type = " em partição)";
                    }
                    fEmuFile.close();
                }
            }
        }
        return NAND + type;
    }

    // trim from end of string (right)
    inline std::string& rtrim(std::string& s)
    {
        const char* t = "\t\n\r\f\v";
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s)
    {
        const char* t = "\t\n\r\f\v";
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s)
    {
        return ltrim(rtrim(s));
    }

    void removeFileWildCardFromDirectory(const std::string& directory, const std::string& fileWildCard)
    {
        for (const auto & e : std::filesystem::directory_iterator(directory))
            if (e.path().string().find(fileWildCard) != std::string::npos) {
                std::filesystem::remove(e.path());
            }
    }

    bool getGithubJSONBody(std::string url, std::string& packBody)
    {
        nlohmann::ordered_json json;
        download::getRequest(url, json, {"accept: application/vnd.github.v3+json"});

        if (json.find("body") != json.end())
            packBody = json["body"];
        else
            return false;

        return true;
    }

    void createCleanInstallFile()
    {
        std::ofstream file;
        file.open(CLEAN_INSTALL_FLAG, std::ofstream::out | std::ofstream::trunc);
        if (file.is_open())
            file << BASE_FOLDER_NAME << "-updater";
        file.close();
    }

    bool deleteThemeFolders()
    {
        std::string tmp;
        tmp = fmt::format("{}{}", AMS_CONTENTS, "0100000000001000");
        std::filesystem::remove_all(tmp.c_str());

        tmp = fmt::format("{}{}", AMS_CONTENTS, "0100000000001007");
        std::filesystem::remove_all(tmp.c_str());

        tmp = fmt::format("{}{}", AMS_CONTENTS, "0100000000001013");
        std::filesystem::remove_all(tmp.c_str());

        return true;
    }
}  // namespace util