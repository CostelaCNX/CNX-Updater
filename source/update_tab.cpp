#include "update_tab.hpp"

#include <filesystem>
#include <string>

#include "confirm_page.hpp"
#include "current_cfw.hpp"
#include "dialogue_page.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

UpdateTab::UpdateTab(std::string& version, std::string& changelog) : brls::List()
{
//    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("menus/common/update_label"_i18n, APP_SHORT_NAME, version, changelog), true);
    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("menus/common/update_label"_i18n, version, changelog), true);
    this->description->setHorizontalAlign(NVG_ALIGN_LEFT);
    this->addView(description);

    CreateDownloadItems();
}

void UpdateTab::CreateDownloadItems()
{
	this->image = (new brls::Image("romfs:/updating.png"));

    std::string operation("menus/common/updating"_i18n);

    listItem = new brls::ListItem("menus/common/update_button"_i18n);
    listItem->setHeight(LISTITEM_HEIGHT);
    listItem->getClickEvent()->subscribe([this, operation](brls::View* view) {
        CreateStagedFrames(operation);
    });
    this->addView(image);
    this->addView(listItem);
}

void UpdateTab::CreateStagedFrames(const std::string& operation)
{
    brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
    stagedFrame->setTitle(operation);

    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, []() { util::downloadArchive(fmt::format(APP_URL, GITHUB_USER, BASE_FOLDER_NAME, BASE_FOLDER_NAME), contentType::app); }, true));
    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, []() { util::extractArchive(contentType::app); }, true));
    stagedFrame->addStage(new ConfirmPage(stagedFrame, "menus/common/app_update_done"_i18n, false, false, false, true));
    brls::Application::pushView(stagedFrame);
}

void UpdateTab::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    this->image->setWidth(112);
    this->image->setHeight(102);
    this->image->setBoundaries(
        this->x + this->width / 2 - this->image->getWidth() / 2,
        96,
        this->image->getWidth(),
        this->image->getHeight());
    this->image->invalidate(true);

    this->description->setWidth(this->width);
    this->description->setHeight(80);
    this->description->setBoundaries(
        this->x + this->width / 2 - this->description->getWidth() / 2,
        210,
        this->description->getWidth(),
        this->description->getHeight());
    this->description->invalidate(true);

    this->listItem->setWidth(700);
    this->listItem->setHeight(LISTITEM_HEIGHT);
    this->listItem->setBoundaries(
        this->x + (this->width / 2) - (this->listItem->getWidth() / 2),
        582,
        this->listItem->getWidth(),
        this->listItem->getHeight());
    this->listItem->invalidate(true);
}

void UpdateTab::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    this->description->frame(ctx);
    this->image->frame(ctx);
    this->listItem->frame(ctx);
}
