#include "ams_tab.hpp"

#include <filesystem>
#include <string>

#include "confirm_page.hpp"
#include "list_download_tab_confirmation.hpp"
#include "current_cfw.hpp"
#include "dialogue_page.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

AmsTab::AmsTab(const nlohmann::json& nxlinks, const bool erista) : brls::List()
{
    this->erista = erista;
    auto cfws = util::getValueFromKey(nxlinks, "cfws");

    std::string packVersion = util::getGMPackVersion();

    std::string sNANDType = util::getNANDType(CurrentCfw::getAmsInfo());
    
    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION,
        fmt::format("menus/ams_update/pack_label"_i18n, util::upperCase(BASE_FOLDER_NAME), BRAND_ARTICLE, BRAND_FULL_NAME) + "\n" +
        fmt::format("menus/ams_update/current_ams"_i18n, (packVersion != "" ? packVersion + " | " : "")) + 
        (CurrentCfw::running_cfw == CFW::ams ? "AMS " + sNANDType : "Outros") +
        (erista ? "\n" + "menus/ams_update/erista_rev"_i18n : "\n" + "menus/ams_update/mariko_rev"_i18n), true);

    this->addView(description);

    CreateDownloadItems(util::getValueFromKey(cfws, util::upperCase(BASE_FOLDER_NAME)), util::upperCase(BASE_FOLDER_NAME), packVersion);

    if (SHOW_GNX)
    {
        description = new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/ams_update/goma_label"_i18n, true);
        this->addView(description);
        CreateDownloadItems(util::getValueFromKey(cfws, "GNX"), "GNX");
    }
}

void AmsTab::CreateDownloadItems(const nlohmann::ordered_json& cfw_links, const std::string& pack, const std::string& sVer)
{
    std::string operation(fmt::format("menus/main/getting"_i18n, pack));
    std::vector<std::pair<std::string, std::string>> links;
    links = download::getLinksFromJson(cfw_links);
    if (links.size()) {
        for (const auto& link : links) {
            std::string json = link.second;
            std::string name;
            std::string url;
            int size;
            std::string body;
            if (util::getLatestCFWPack(json, name, url, size, body))
            {
                int iSize = (size / 1048576);

                if (sVer.find(pack) != std::string::npos)
                    if (name.find(sVer) != std::string::npos)
                        listItem = new brls::ListItem(fmt::format("{}{} ({}MB)", "\u2605 ", name, iSize));
                    else
                        listItem = new brls::ListItem(fmt::format("{}{} ({}MB)", "\uE150 ", name, iSize));
                else
                    listItem = new brls::ListItem(fmt::format("{} ({}MB)", name, iSize));

                listItem->setHeight(LISTITEM_HEIGHT);
                listItem->getClickEvent()->subscribe([this, name, url, iSize, pack, body, operation](brls::View* view) {
                    if (!erista && !std::filesystem::exists(MARIKO_PAYLOAD_PATH)) {
                        brls::Application::crash("menus/errors/mariko_payload_missing"_i18n);
                    }
                    else {
                        CreateStagedFrames("menus/common/download"_i18n + name, url, iSize, pack, body, operation, erista);
                    }
                });
                this->addView(listItem);
            }
        }
    }
    else {
        description = new brls::Label(
            brls::LabelStyle::SMALL,
            "menus/main/links_not_found"_i18n,
            true);
        description->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(description);
    }

}

void AmsTab::CreateStagedFrames(const std::string& text, const std::string& url, const int& size, const std::string& pack, const std::string& body, const std::string& operation, bool erista)
{
    brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
    stagedFrame->setTitle(operation);

    if ((body != "") && (util::upperCase(body).find("[PROBLEMAS CONHECIDOS]", 0) != std::string::npos))
        stagedFrame->addStage(new ListDownloadConfirmationPage(stagedFrame, DialogType::warning, "menus/main/download_time_warning"_i18n, pack, body, true));
    else
        stagedFrame->addStage(new ListDownloadConfirmationPage(stagedFrame, DialogType::warning, "menus/main/download_time_warning"_i18n, pack, body, false));

//    stagedFrame->addStage(new ConfirmPage(stagedFrame, fmt::format("{} ({}MB)", text, size)));
    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url]() { util::downloadArchive(url, contentType::ams_cfw); }));
    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, []() { util::extractArchive(contentType::ams_cfw); }));
    stagedFrame->addStage(new ConfirmPage(stagedFrame, "menus/ams_update/reboot_rcm"_i18n, false, true, erista));
    brls::Application::pushView(stagedFrame);
}
