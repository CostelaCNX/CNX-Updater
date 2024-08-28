#pragma once

#include <switch.h>

#include <algorithm>
#include <borealis.hpp>
#include <filesystem>
#include <json.hpp>
#include <set>

static constexpr uint32_t MaxTitleCount = 64000;

enum class appPageType
{
    base,
    cheatSlips,
    gbatempCheats
};

class AppPage : public brls::AppletFrame
{
private:
    brls::ListItem* download;
    std::set<std::string> titles;

protected:
    brls::List* list;
    brls::Label* label;
    brls::ListItem* listItem;
    uint64_t GetCurrentApplicationId();
    u32 InitControlData(NsApplicationControlData** controlData);
    uint32_t GetControlData(u64 tid, NsApplicationControlData* controlData, u64& controlSize, std::string& name);
    virtual void PopulatePage();
    virtual void CreateLabel(){};
    virtual void AddListItem(const std::string& name, uint64_t tid);

public:
    AppPage();
};
