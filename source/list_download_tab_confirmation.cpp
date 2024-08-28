#include "list_download_tab_confirmation.hpp"

#include <algorithm>
#include <filesystem>
#include <string>

#include "fs.hpp"
#include "main_frame.hpp"
#include "utils.hpp"
#include "changelog_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
ListDownloadConfirmationPage::ListDownloadConfirmationPage(brls::StagedAppletFrame* frame, const DialogType dialogType, const std::string& text, const std::string& pack, const std::string& body, bool showChangelog, bool done) : done(done)
{
    this->showChangelog = showChangelog;
    this->packName = pack;
    this->dialogType = dialogType;

    this->icon = (new brls::Image("romfs:/gui_icon.png"));
    this->image = (new brls::Image(ROMFSIconFile[(int)dialogType].data()));
    this->label = new brls::Label(brls::LabelStyle::REGULAR, text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_LEFT);
    this->label->setParent(this);

    this->button = (new brls::Button(brls::ButtonStyle::REGULAR))->setLabel(done ? "menus/common/back"_i18n : "menus/common/continue"_i18n);
    this->button->setParent(this);
    this->button->getClickEvent()->subscribe([frame, this](View* view) {
        if (!frame->isLastStage()) {
            frame->nextStage();
        }
        else if (this->done) {
            brls::Application::pushView(new MainFrame());
        }
    });

    if ((this->showChangelog) || (this->dialogType == DialogType::error))
    {
        this->button2 = (new brls::Button(brls::ButtonStyle::REGULAR));
        this->button2->setParent(this);

        if (this->showChangelog)
        {
            this->button2->getClickEvent()->subscribe([frame, pack, body](View* view) {
                int index = util::upperCase(body).find("[PROBLEMAS CONHECIDOS]", 0);
                std::string sChangelog = body.substr(body.find("]", 0) + 1, (index - 11));
                std::string sKnownIssues = body.substr(index + 22, (body.length() - index));

                brls::TabFrame* popupChangelog = new brls::TabFrame();
                popupChangelog->addTab("menus/changelog/changelog"_i18n, new brls::Label(brls::LabelStyle::REGULAR, sChangelog, true)   );
                popupChangelog->addTab("menus/changelog/known_issues"_i18n, new brls::Label(brls::LabelStyle::REGULAR, sKnownIssues, true)   );
                brls::PopupFrame::open(fmt::format("{} no {}", "menus/changelog/changelog"_i18n, pack), popupChangelog, "menus/changelog/changelog"_i18n, "menus/changelog/known_issues"_i18n);
            });

            this->navigationMap.add(this->button, brls::FocusDirection::RIGHT, this->button2);
            this->navigationMap.add(this->button2, brls::FocusDirection::LEFT, this->button);
        }
        else if (this->dialogType == DialogType::error)
        {
            this->button2->getClickEvent()->subscribe([frame](View* view) {
                if (util::deleteThemeFolders())
                    frame->nextStage();
            });

            this->button3 = (new brls::Button(brls::ButtonStyle::REGULAR));
            this->button3->setParent(this);
            this->button3->getClickEvent()->subscribe([](View* view) {
                brls::Application::popView();
            });

            this->navigationMap.add(this->button, brls::FocusDirection::RIGHT, this->button2);
            this->navigationMap.add(this->button2, brls::FocusDirection::LEFT, this->button);
            this->navigationMap.add(this->button2, brls::FocusDirection::RIGHT, this->button3);
            this->navigationMap.add(this->button3, brls::FocusDirection::LEFT, this->button2);
        }
    }

    if (this->done)
        this->registerAction("", brls::Key::B, [this] { return true; });
}

void ListDownloadConfirmationPage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    if (!this->done) {
        auto end = std::chrono::high_resolution_clock::now();
        auto missing = std::max(1l - std::chrono::duration_cast<std::chrono::seconds>(end - start).count(), 0l);
        auto b1Text = std::string("menus/common/continue"_i18n);
        auto b2Text = std::string("menus/common/remove_continue"_i18n);
        if (missing > 0) {
            this->button->setLabel(b1Text + " (" + std::to_string(missing) + ")");
            this->button->setState(brls::ButtonState::DISABLED);

            if (this->dialogType == DialogType::error)
            {
                this->button2->setLabel(b2Text + " (" + std::to_string(missing) + ")");
                this->button2->setState(brls::ButtonState::DISABLED);
            }
        }
        else {
            this->button->setLabel(b1Text);
            this->button->setState(brls::ButtonState::ENABLED);

            if (this->dialogType == DialogType::error)
            {
                this->button2->setLabel(b2Text);
                this->button2->setState(brls::ButtonState::ENABLED);
            }
        }
        this->button->invalidate();
		if (this->dialogType == DialogType::error)
			this->button2->invalidate();

        if (this->showChangelog)
        {        
            this->button2->setLabel(fmt::format("{} no {}", "menus/changelog/changelog"_i18n, this->packName));
            this->button2->setState(brls::ButtonState::ENABLED);
            this->button2->invalidate();
        }
        else if (this->dialogType == DialogType::error)
        {        
//            this->button2->setLabel("menus/common/remove_continue"_i18n);
//            this->button2->setState(brls::ButtonState::ENABLED);
//            this->button2->invalidate();

            this->button3->setLabel("menus/common/cancel"_i18n);
            this->button3->setState(brls::ButtonState::ENABLED);
            this->button3->invalidate();
        }
    }
    else {
        this->button->setState(brls::ButtonState::ENABLED);
    }

    this->icon->frame(ctx);
    this->image->frame(ctx);
    this->label->frame(ctx);
    this->button->frame(ctx);

    if (this->showChangelog)
        this->button2->frame(ctx);
    else if (this->dialogType == DialogType::error)
    {
        this->button2->frame(ctx);
        this->button3->frame(ctx);
    }
}

brls::View* ListDownloadConfirmationPage::getDefaultFocus()
{
    return this->button;
}

brls::View* ListDownloadConfirmationPage::getNextFocus(brls::FocusDirection direction, brls::View* currentView)
{
    return this->navigationMap.getNextFocus(direction, currentView);
}

void ListDownloadConfirmationPage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    //page icon
    this->icon->setWidth(52);
    this->icon->setHeight(52);
    this->icon->setBoundaries(style->AppletFrame.imageLeftPadding, style->AppletFrame.imageTopPadding, style->AppletFrame.imageSize, style->AppletFrame.imageSize);
    this->icon->invalidate(true);

    //watrning image
    this->image->setWidth(124);
    this->image->setHeight(112);
    this->image->setBoundaries(
        this->x + this->width / 2 - this->image->getWidth() / 2,
        96,
        this->image->getWidth(),
        this->image->getHeight());
    this->image->invalidate(true);
    
    this->label->setWidth(this->width);
    this->label->invalidate(true);
    // this->label->setBackground(brls::ViewBackground::DEBUG);
    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        //this->y + (this->height - this->label->getHeight() - this->y - style->CrashFrame.buttonHeight) / 2,
        214,
        this->label->getWidth(),
        this->label->getHeight());

    if (this->showChangelog)
    {
        this->button->setBoundaries(
            this->x + (this->width / 2) - (style->CrashFrame.buttonWidth / 2) - (this->width / 5),
            this->y + this->height - (style->CrashFrame.buttonHeight * 1.25),
            style->CrashFrame.buttonWidth,
            style->CrashFrame.buttonHeight);
        this->button->invalidate();
    
        this->button2->setBoundaries(
            this->x + (this->width / 2) - (style->CrashFrame.buttonWidth / 2) + (this->width / 5),
            this->y + this->height - (style->CrashFrame.buttonHeight * 1.25),
            style->CrashFrame.buttonWidth,
            style->CrashFrame.buttonHeight);
        this->button2->invalidate();
    }
    else if (this->dialogType == DialogType::error)
    {
        this->button->setBoundaries(
            this->x + 32,
            this->y + this->height - (style->CrashFrame.buttonHeight * 1.25),
            style->CrashFrame.buttonWidth,
            style->CrashFrame.buttonHeight);
        this->button->invalidate();
    
        this->button2->setBoundaries(
            this->x + (this->width / 2) - (style->CrashFrame.buttonWidth / 2),
            this->y + this->height - (style->CrashFrame.buttonHeight * 1.25),
            style->CrashFrame.buttonWidth,
            style->CrashFrame.buttonHeight);
        this->button2->invalidate();

        this->button3->setBoundaries(
            this->x + this->width - style->CrashFrame.buttonWidth - 32,
            this->y + this->height - (style->CrashFrame.buttonHeight * 1.25),
            style->CrashFrame.buttonWidth,
            style->CrashFrame.buttonHeight);
        this->button3->invalidate();
    }
    else
    {
        this->button->setBoundaries(
            this->x + (this->width / 2) - (style->CrashFrame.buttonWidth / 2),
            this->y + this->height - (style->CrashFrame.buttonHeight * 1.25),
            style->CrashFrame.buttonWidth,
            style->CrashFrame.buttonHeight);
        this->button->invalidate();        
    }
    start = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(150);
}

ListDownloadConfirmationPage::~ListDownloadConfirmationPage()
{
    delete this->icon;
    delete this->label;
    delete this->button;
    delete this->image;
    if (this->showChangelog)
        delete this->button2;
    if  (this->dialogType == DialogType::error)
    {
        delete this->button2;
        delete this->button3;
    }
}