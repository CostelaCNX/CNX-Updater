#pragma once

#include <borealis.hpp>

class IconsPage : public brls::AppletFrame
{
private:
    brls::List* list;
    brls::ListItem* listItem;

public:
    IconsPage();
    void ShowChangelogContent(const std::string version, const std::string content);
};