#pragma once

#include <borealis.hpp>
#include <json.hpp>
#include <set>

class UpdateTab : public brls::List
{
private:
    brls::ListItem* listItem;
    brls::Label* description;
    brls::Image* image = nullptr;
    void CreateDownloadItems();
    void CreateStagedFrames(const std::string& operation);

public:
    UpdateTab(std::string& version, std::string& changelog);

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
};
