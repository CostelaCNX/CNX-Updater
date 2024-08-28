#pragma once

#include <borealis.hpp>
#include <json.hpp>

class ToolsTab : public brls::List
{
private:
    
    brls::StagedAppletFrame* stagedFrame;

public:
    ToolsTab(const std::string& tag, bool erista = true);
};