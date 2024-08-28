#include "list_download_tab.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "app_page.hpp"
#include "confirm_page.hpp"
#include "list_download_tab_confirmation.hpp"
#include "dialogue_page.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "utils.hpp"
#include "worker_page.hpp"


namespace i18n = brls::i18n;
using namespace i18n::literals;

ListDownloadTab::ListDownloadTab(const contentType type, const nlohmann::ordered_json& nxlinks) : brls::List(), type(type), nxlinks(nxlinks)
{
    SetSysFirmwareVersion ver;
    std::string sVer = R_SUCCEEDED(setsysGetFirmwareVersion(&ver)) ? ver.display_version : "menus/main/not_found"_i18n;

    this->setDescription(sVer);

    this->createList(sVer);
}

void ListDownloadTab::createList(std::string& sVer)
{
    ListDownloadTab::createList(this->type, sVer);
}

void ListDownloadTab::createList(contentType type, std::string& sVer)
{
    std::vector<std::pair<std::string, std::string>> links;
    nlohmann::ordered_json firm;
    std::string url;

    switch (type) {
        case contentType::fw: {
            firm = util::getValueFromKey(this->nxlinks, "firmwares");
            if (!firm.empty()) {
                url = firm["releases"];
                links = download::getLinksFromGitHubReleases(url, MAX_FETCH_LINKS);
            }
            else
                links.clear();
            break;
        }
        default:
            links = download::getLinksFromJson(util::getValueFromKey(this->nxlinks, contentTypeNames[(int)type].data()));
            break;
    }

    if (links.size()) {
        for (const auto& link : links) {
            const std::string title = link.first;
            const std::string url = link.second;
            const std::string text("menus/common/download"_i18n + title);

            if (title.find(sVer) != std::string::npos)
                listItem = new brls::ListItem(fmt::format("{}{}", "\u2605 ", title));
            else
                listItem = new brls::ListItem(title);

            listItem->setHeight(LISTITEM_HEIGHT);
            listItem->getClickEvent()->subscribe([this, type, text, url, title](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(fmt::format("menus/main/getting"_i18n, contentTypeFullNames[(int)type].data()));

                switch (type) {
                    case contentType::fw: {
                        std::string contentsPath = util::getContentsPath();
                        for (const auto& tid : {"0100000000001000", "0100000000001007", "0100000000001013"}) {
                            if (std::filesystem::exists(contentsPath + tid) && !std::filesystem::is_empty(contentsPath + tid)) {
                                stagedFrame->addStage(new ListDownloadConfirmationPage(stagedFrame, DialogType::error, fmt::format("menus/main/theme_warning"_i18n, AMS_CONTENTS, AMS_CONTENTS, AMS_CONTENTS), "", "", false));
                                break;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }

                stagedFrame->addStage(new ListDownloadConfirmationPage(stagedFrame, DialogType::warning, "menus/main/download_time_warning"_i18n, "", "", false));
//                stagedFrame->addStage(new ConfirmPage(stagedFrame, text));
                stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [this, type, url]() { util::downloadArchive(url, type); }));
                stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [this, type]() { util::extractArchive(type); }));
                
                std::string doneMsg = "menus/common/all_done"_i18n;
                switch (type) {
                    case contentType::fw: {
/*
                        std::string contentsPath = util::getContentsPath();
                        for (const auto& tid : {"0100000000001000", "0100000000001007", "0100000000001013"}) {
                            if (std::filesystem::exists(contentsPath + tid) && !std::filesystem::is_empty(contentsPath + tid)) {
                                doneMsg += "\n" + "menus/main/theme_warning"_i18n;
                                break;
                            }
                        }
*/
                        if (std::filesystem::exists(DAYBREAK_PATH)) {
                            stagedFrame->addStage(new DialoguePage_fw(stagedFrame, doneMsg));
                        }
                        else {
                            stagedFrame->addStage(new ConfirmPage(stagedFrame, doneMsg, true));
                        }
                        break;
                    }
                    default:
                        stagedFrame->addStage(new ConfirmPage(stagedFrame, doneMsg, true));
                        break;
                }
                brls::Application::pushView(stagedFrame);
            });
            this->addView(listItem);
        }
    }
    else {
        this->displayNotFound();
    }
}

void ListDownloadTab::displayNotFound()
{
    brls::Label* notFound = new brls::Label(
        brls::LabelStyle::SMALL,
        "menus/main/links_not_found"_i18n,
        true);
    notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(notFound);
}

void ListDownloadTab::setDescription(std::string& sVer)
{
    this->setDescription(this->type, sVer);
}

void ListDownloadTab::setDescription(contentType type, std::string& sVer)
{
    brls::Label* description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);

    switch (type) {
        case contentType::fw: {
            description->setText(fmt::format("menus/main/firmware_text"_i18n, sVer, MAX_FETCH_LINKS));
            break;
        }
        default:
            break;
    }

    this->addView(description);
}
