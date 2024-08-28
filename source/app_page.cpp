#include "app_page.hpp"

#include <switch.h>

#include <filesystem>
#include <fstream>
#include <regex>

#include "confirm_page.hpp"
#include "current_cfw.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

AppPage::AppPage() : AppletFrame(true, true)
{
    list = new brls::List();
}

void AppPage::PopulatePage()
{
    this->CreateLabel();

    NsApplicationRecord* records = new NsApplicationRecord[MaxTitleCount];
    NsApplicationControlData* controlData = NULL;
    std::string name;

    s32 recordCount = 0;
    u64 controlSize = 0;
    u64 tid;

    if (!util::isApplet()) {
        if (R_SUCCEEDED(nsListApplicationRecord(records, MaxTitleCount, 0, &recordCount))) {
            for (s32 i = 0; i < recordCount; i++) {
                controlSize = 0;

                if (R_FAILED(InitControlData(&controlData))) break;

                tid = records[i].application_id;

                if (R_FAILED(GetControlData(tid, controlData, controlSize, name)))
                    continue;

                listItem = new brls::ListItem(name, "", util::formatApplicationId(tid));
                listItem->setThumbnail(controlData->icon, sizeof(controlData->icon));
                this->AddListItem(name, tid);
            }
            free(controlData);
        }
    }
    else {
        tid = GetCurrentApplicationId();
        if (R_SUCCEEDED(InitControlData(&controlData)) && R_SUCCEEDED(GetControlData(tid & 0xFFFFFFFFFFFFF000, controlData, controlSize, name))) {
            this->AddListItem(name, tid);
        }
        label = new brls::Label(brls::LabelStyle::SMALL, fmt::format("menus/common/applet_mode_not_supported"_i18n, APP_FULL_NAME), true);
        list->addView(label);
    }
    delete[] records;

    brls::Logger::debug("count {}", list->getViewsCount());

    if(!list->getViewsCount()) {
        list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/common/nothing_to_see"_i18n, true));
    }

    this->setContentView(list);
}

u32 AppPage::InitControlData(NsApplicationControlData** controlData)
{
    free(*controlData);
    *controlData = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
    if (*controlData == NULL) {
        return 300;
    }
    else {
        memset(*controlData, 0, sizeof(NsApplicationControlData));
        return 0;
    }
}

u32 AppPage::GetControlData(u64 tid, NsApplicationControlData* controlData, u64& controlSize, std::string& name)
{
    Result rc;
    NacpLanguageEntry* langEntry = NULL;
    rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, controlData, sizeof(NsApplicationControlData), &controlSize);
    if (R_FAILED(rc)) return rc;

    if (controlSize < sizeof(controlData->nacp)) return 100;

    rc = nacpGetLanguageEntry(&controlData->nacp, &langEntry);
    if (R_FAILED(rc)) return rc;

    if (!langEntry->name) return 200;

    name = langEntry->name;

    return 0;
}

void AppPage::AddListItem(const std::string& name, u64 tid)
{
    list->addView(listItem);
}

uint64_t AppPage::GetCurrentApplicationId()
{
    Result rc = 0;
    uint64_t pid = 0;
    uint64_t tid = 0;

    rc = pmdmntGetApplicationProcessId(&pid);
    if (rc == 0x20f || R_FAILED(rc)) return 0;

    rc = pminfoGetProgramId(&tid, pid);
    if (rc == 0x20f || R_FAILED(rc)) return 0;

    return tid;
}
