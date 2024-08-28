#include "list_extra_tab.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "confirm_page.hpp"
#include "list_download_tab_confirmation.hpp"
#include "download.hpp"
#include "utils.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

ListExtraTab::ListExtraTab(const contentType type, const nlohmann::ordered_json& nxlinks) : brls::List(), type(type), nxlinks(nxlinks)
{
    this->setDescription();

    this->createList();
}

void ListExtraTab::createList()
{
    ListExtraTab::createList(this->type);
}

void ListExtraTab::createList(contentType type)
{
    int counter = 0;

    std::string sInstalledFile = "";
    int sInstalledSize = -1;
    bool bInstalled;

    std::string path = util::getContentsPath();
    std::vector<std::string> itemFolders;

    const nlohmann::ordered_json jsonExtras = util::getValueFromKey(this->nxlinks, contentTypeNames[(int)type].data());
    if (jsonExtras.size()) {
        for (auto it = jsonExtras.begin(); it != jsonExtras.end(); ++it)
        {            
            bool enabled = (*it)["enabled"].get<bool>();
            
            if (enabled) {
                //resetting variables
                sInstalledFile = "";
                sInstalledSize = -1;
                bInstalled = false;

                //getting values from JSON
                const std::string title = it.key();
                const std::string url = (*it)["link"].get<std::string>();
                const std::string size = (*it)["size"].get<std::string>();

                //checking whether the content exists in disk
                if ((*it)["installed"].contains("hash_file"))
                {
                    sInstalledFile = path + (*it)["installed"]["hash_file"].get<std::string>();
                    sInstalledSize = (*it)["installed"]["hash_size"].get<int>();

                    if (std::filesystem::exists(sInstalledFile))
                    {
                        FILE *p_file = NULL;
                        p_file = fopen(sInstalledFile.c_str(),"rb");
                        fseek(p_file,0,SEEK_END);
                        int size = ftell(p_file);
                        fclose(p_file);

                        if (size == sInstalledSize)
                            bInstalled = true;
                        else
                            bInstalled = false;
                    }
                }

                //getting the list of files to delete
                if ((*it)["installed"].contains("uninstall"))
                {
                    itemFolders.clear();
                    
                    for (auto it2 = (*it)["installed"]["uninstall"].begin(); it2 != (*it)["installed"]["uninstall"].end(); ++it2)
                    {
                        const std::string tmp = it2.value();
                        itemFolders.push_back(tmp);
                        
                        if (sInstalledSize < 0)
                            bInstalled = std::filesystem::exists(path + tmp);
                    }
                }
                
                const std::string text("menus/common/download"_i18n + title);
                listItem = new brls::ListItem(fmt::format("{}{} ({})", (bInstalled ? "\u2605 " : ""), title, size));
                listItem->setHeight(LISTITEM_HEIGHT);
                listItem->getClickEvent()->subscribe([this, type, text, url, itemFolders, bInstalled](brls::View* view) {
                    brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                    stagedFrame->setTitle(fmt::format("menus/main/getting"_i18n, contentTypeFullNames[(int)type].data()));

                    if (bInstalled)
                    {
                        switch (type) {
                            case contentType::translations: {
                                stagedFrame->addStage(new ListDownloadConfirmationPage(stagedFrame, DialogType::warning, "menus/main/translation_exists_warning"_i18n, "", "", false));
                                break;
                            }
                            case contentType::modifications: {
                                stagedFrame->addStage(new ListDownloadConfirmationPage(stagedFrame, DialogType::warning, "menus/main/modification_exists_warning"_i18n, "", "", false));
                                break;
                            }
                                default:
                                break;
                        }

                        stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/deleting"_i18n, [this, type, itemFolders]() { util::doDelete(itemFolders); }));
                    }
                    else
                    {
                        stagedFrame->addStage(new ConfirmPage(stagedFrame, text));
                        stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [this, type, url]() { util::downloadArchive(url, type); }));
                        stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [this, type]() { util::extractArchive(type); }));
                    }

                    stagedFrame->addStage(new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true));
                    brls::Application::pushView(stagedFrame);
                });
                this->addView(listItem);

                counter++;
            }
        }

        if (counter <= 0)
            this->noItemsToDisplay();
    }
    else {
        this->displayNotFound();
    }
}

void ListExtraTab::displayNotFound()
{
    brls::Label* notFound = new brls::Label(
        brls::LabelStyle::SMALL,
        "menus/main/links_not_found"_i18n,
        true);
    notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(notFound);
}

void ListExtraTab::noItemsToDisplay()
{
    brls::Label* notFound = new brls::Label(
        brls::LabelStyle::SMALL,
        "menus/main/no_items_to_display"_i18n,
        true);
    notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(notFound);
}

void ListExtraTab::setDescription()
{
    this->setDescription(this->type);
}

void ListExtraTab::setDescription(contentType type)
{
    brls::Label* description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);

    switch (type) {
        case contentType::translations: {
            description->setText("menus/main/translations_text"_i18n);
            break;
        }
        case contentType::modifications: {
            description->setText("menus/main/modifications_text"_i18n);
            break;
        }
        default:
            break;
    }

    this->addView(description);
}
