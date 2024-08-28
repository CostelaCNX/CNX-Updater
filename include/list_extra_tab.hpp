#pragma once

#include <borealis.hpp>
#include <json.hpp>

#include "constants.hpp"

class ListExtraTab : public brls::List
{
private:
    brls::ListItem* listItem;
    nlohmann::ordered_json nxlinks;
    contentType type;
    int size = 0;
    void createList();
    void createList(contentType type);
    void setDescription();
    void setDescription(contentType type);
    void displayNotFound();
    void noItemsToDisplay();

public:
    ListExtraTab(const contentType type, const nlohmann::ordered_json& nxlinks = nlohmann::ordered_json::object());
};