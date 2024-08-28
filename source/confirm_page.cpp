#include "confirm_page.hpp"

#include <algorithm>
#include <filesystem>
#include <string>

#include "fs.hpp"
#include "main_frame.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
ConfirmPage::ConfirmPage(brls::StagedAppletFrame* frame, const std::string& text, bool done, bool reboot, bool erista, const bool& isUpdating) : done(done), reboot(reboot), erista(erista)
{
    this->icon = (new brls::Image("romfs:/gui_icon.png"));

    this->button = (new brls::Button(brls::ButtonStyle::REGULAR))->setLabel(done ? "menus/common/back"_i18n : "menus/common/continue"_i18n);
    this->button->setParent(this);
    this->button->getClickEvent()->subscribe([frame, this](View* view) {
        if (!frame->isLastStage()) {
            frame->nextStage();
        }
        else if (this->done) {
            brls::Application::pushView(new MainFrame());
        }
        else if (this->reboot) {
            if (this->erista) {
                util::rebootToPayload(RCM_PAYLOAD_PATH);
            }
            else {
                if (std::filesystem::exists(UPDATE_BIN_PATH)) {
                    fs::copyFile(UPDATE_BIN_PATH, MARIKO_PAYLOAD_PATH_TEMP);
                }
                else {
                    fs::copyFile(REBOOT_PAYLOAD_PATH, MARIKO_PAYLOAD_PATH_TEMP);
                }
                fs::copyFile(RCM_PAYLOAD_PATH, MARIKO_PAYLOAD_PATH);
                util::shutDown(true);
            }
        }
    });

    this->label = new brls::Label(brls::LabelStyle::DIALOG, text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);

    if (!isUpdating)
    {
        if (this->done || this->reboot)
            this->registerAction("", brls::Key::B, [this] { return true; });
    }
    else
    {
        if (this->done || this->reboot)
		{
		    this->registerAction("", brls::Key::A, [this] { return true; });
            this->registerAction("", brls::Key::B, [this] { return true; });
            this->registerAction("", brls::Key::PLUS, [this] { return true; });
        }
    }
}

void ConfirmPage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    if (!this->done) {
        auto end = std::chrono::high_resolution_clock::now();
        auto missing = std::max(1l - std::chrono::duration_cast<std::chrono::seconds>(end - start).count(), 0l);
        auto text = std::string("menus/common/continue"_i18n);
        if (missing > 0) {
            this->button->setLabel(text + " (" + std::to_string(missing) + ")");
            this->button->setState(brls::ButtonState::DISABLED);
        }
        else {
            this->button->setLabel(text);
            this->button->setState(brls::ButtonState::ENABLED);
        }
        this->button->invalidate();
    }
    else {
        this->button->setState(brls::ButtonState::ENABLED);
    }
    this->icon->frame(ctx);
    this->label->frame(ctx);
    this->button->frame(ctx);
}

brls::View* ConfirmPage::getDefaultFocus()
{
    return this->button;
}

void ConfirmPage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    //page icon
    this->icon->setWidth(52);
    this->icon->setHeight(52);
    this->icon->setBoundaries(style->AppletFrame.imageLeftPadding, style->AppletFrame.imageTopPadding, style->AppletFrame.imageSize, style->AppletFrame.imageSize);
    this->icon->invalidate(true);

    this->label->setWidth(this->width);
    this->label->invalidate(true);
    // this->label->setBackground(brls::ViewBackground::DEBUG);
    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - this->label->getHeight() - this->y - style->CrashFrame.buttonHeight) / 2,
        this->label->getWidth(),
        this->label->getHeight());

    this->button->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2,
        //this->y + (this->height - style->CrashFrame.buttonHeight * 3),
		this->y + (this->height - (style->CrashFrame.buttonHeight * 1.25)),
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->button->invalidate();

    start = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(150);
}

ConfirmPage::~ConfirmPage()
{
    delete this->icon;
    delete this->label;
    delete this->button;
}