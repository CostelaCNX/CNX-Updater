#include "credits_tab.hpp"
#include "constants.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

CreditsTab::CreditsTab()
{
    brls::Label* text = new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("menus/credits/title_main"_i18n, APP_FULL_NAME, APP_SHORT_NAME), true);
    this->addView(text);

    // Credits
    this->addView(new brls::Header("menus/credits/title_credits"_i18n));
    brls::Label* credits = new brls::Label(brls::LabelStyle::SMALL, fmt::format("menus/credits/credits_text"_i18n, APP_SHORT_NAME), true);
    this->addView(credits);

    // Beta testers
    this->addView(new brls::Header("menus/credits/title_beta"_i18n));
    brls::Label* beta = new brls::Label(brls::LabelStyle::SMALL, "menus/credits/beta_text"_i18n, true);
    this->addView(beta);
}