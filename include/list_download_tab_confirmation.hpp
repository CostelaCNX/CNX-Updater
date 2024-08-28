#pragma once

#include <borealis.hpp>
#include <chrono>

#include "constants.hpp"

class ListDownloadConfirmationPage : public brls::View
{
private:
    brls::Button* button = nullptr;
    brls::Button* button2 = nullptr;
    brls::Button* button3 = nullptr;
    brls::Label* label = nullptr;
	brls::Image* icon = nullptr;
	brls::Image* image = nullptr;
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    bool done = false;
    brls::NavigationMap navigationMap;
    bool showChangelog = false;
    std::string packName;
    DialogType dialogType;

public:
    ListDownloadConfirmationPage(brls::StagedAppletFrame* frame, const DialogType dialogType, const std::string& text, const std::string& pack = "", const std::string& body = "", bool showChangelog = false, bool done = false);
    ~ListDownloadConfirmationPage();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
    brls::View* getDefaultFocus() override;
    brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView);
};