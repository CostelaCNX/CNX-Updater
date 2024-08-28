#include "main_frame.hpp"

#include <fstream>
#include <json.hpp>

#include "about_tab.hpp"
#include "ams_tab.hpp"
#include "update_tab.hpp"
#include "download.hpp"
#include "fs.hpp"
#include "list_download_tab.hpp"
#include "list_extra_tab.hpp"
#include "credits_tab.hpp"
#include "tools_tab.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

namespace {
    constexpr const char AppTitle[] = APP_TITLE;
    constexpr const char AppVersion[] = APP_VERSION;
}  // namespace

MainFrame::MainFrame() : TabFrame()
{
    this->setIcon("romfs:/gui_icon.png");
    this->setTitle(AppTitle);

    s64 freeStorage;
    std::string tag = util::getLatestTag();

    bool newversion = false;
    
    if (!tag.empty()) {
        //fetching the version as a number
        std::string temp = "";
        int iTag = 0;
        int iAppVersion = 0;

        temp.reserve(tag.size()); // optional, avoids buffer reallocations in the loop
        for (char c : tag)
            if (std::isdigit(c)) temp += c;
        iTag = std::stoi(temp); // casting from string to integer

        temp = "";

        temp.reserve(strlen(AppVersion)); // optional, avoids buffer reallocations in the loop
        for (char c : AppVersion)
            if (std::isdigit(c)) temp += c;
        iAppVersion = std::stoi(temp); // casting from string to integer

        newversion = (iTag > iAppVersion);

        this->setFooterText(fmt::format("menus/main/footer_text"_i18n, BRAND_FULL_NAME,
            "v" + std::string(AppVersion),
            R_SUCCEEDED(fs::getFreeStorageSD(freeStorage)) ? floor(((float)freeStorage / 0x40000000) * 100.0) / 100.0 : -1));
    }
    else {
        this->setFooterText(fmt::format("menus/main/footer_text"_i18n, BRAND_FULL_NAME, std::string(AppVersion) + "menus/main/footer_text_not_connected"_i18n, R_SUCCEEDED(fs::getFreeStorageSD(freeStorage)) ? (float)freeStorage / 0x40000000 : -1));
    }

    nlohmann::ordered_json nxlinks;
    download::getRequest(NXLINKS_URL, nxlinks);

    bool erista = util::isErista();

    this->registerAction("menus/main/help"_i18n, brls::Key::X, [] {
        brls::TabFrame* popupHelp = new brls::TabFrame();
        popupHelp->addTab("menus/main/help_how_to_use"_i18n, new brls::Label(brls::LabelStyle::REGULAR, "menus/main/help_how_to_use_text"_i18n, true));
        popupHelp->addTab("menus/main/help_order"_i18n, new brls::Label(brls::LabelStyle::REGULAR, fmt::format("menus/main/help_order_text"_i18n, "menus/main/update_ams"_i18n, util::upperCase(BASE_FOLDER_NAME), "menus/main/download_firmware"_i18n), true));
        popupHelp->addTab("menus/main/help_clean_inst"_i18n, new brls::Label(brls::LabelStyle::REGULAR, "menus/main/help_clean_inst_text"_i18n, true));
        brls::PopupFrame::open("menus/main/help"_i18n, popupHelp, "menus/main/help_how_to_use_full"_i18n, "");
        return true;
    });

    if (!newversion) {
        this->addTab("menus/main/about"_i18n, new AboutTab());
        this->addTab("menus/main/update_ams"_i18n, new AmsTab(nxlinks, erista));
        this->addTab("menus/main/download_firmware"_i18n, new ListDownloadTab(contentType::fw, nxlinks));
        this->addTab("menus/main/download_translations"_i18n, new ListExtraTab(contentType::translations, nxlinks));
        this->addTab("menus/main/download_mods"_i18n, new ListExtraTab(contentType::modifications, nxlinks));
        this->addTab("menus/main/tools"_i18n, new ToolsTab(tag, erista));
        this->addSeparator();
        this->addTab("menus/main/credits"_i18n, new CreditsTab());

        this->registerAction("", brls::Key::B, [] { return true; });
    }
    else
    {
        std::string changelog;
        util::getGithubJSONBody(fmt::format(APP_INFO, GITHUB_USER, BASE_FOLDER_NAME), changelog);
        changelog.erase( std::remove(changelog.begin(), changelog.end(), '\r'), changelog.end() );
            
        this->addTab("menus/main/new_update"_i18n, new UpdateTab(tag, changelog));
        this->registerAction("", brls::Key::B, [] { return true; });
        this->registerAction("", brls::Key::X, [] { return true; });
    }
}
