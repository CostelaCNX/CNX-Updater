#pragma once

#include <borealis.hpp>
#include <json.hpp>

#include "constants.hpp"

class ListDownloadTab : public brls::List
{
private:
    brls::ListItem* listItem;
    nlohmann::ordered_json nxlinks;
    contentType type;
    int size = 0;
    void createList(std::string& sVer);
    void createList(contentType type, std::string& sVer);
    void setDescription(std::string& sVer);
    void setDescription(contentType type, std::string& sVer);
    void displayNotFound();

public:
    ListDownloadTab(const contentType type, const nlohmann::ordered_json& nxlinks = nlohmann::ordered_json::object());
};