#include "about_tab.hpp"
#include <chrono>
#include "constants.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

AboutTab::AboutTab()
{
    // Subtitle
    brls::Label* subTitle = new brls::Label(
        brls::LabelStyle::REGULAR,
        fmt::format("menus/about/title"_i18n, APP_FULL_NAME, APP_SHORT_NAME),
        true);
    subTitle->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(subTitle);

    // Copyright
    brls::Label* copyright = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        fmt::format("menus/about/copyright"_i18n, APP_FULL_NAME, APP_SHORT_NAME, "2022", BRAND_FULL_NAME),
        true);

    copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(copyright);

    // Links
    this->addView(new brls::Header("Disclaimers"));
    brls::Label* links = new brls::Label(brls::LabelStyle::SMALL, fmt::format("menus/about/disclaimers"_i18n, BRAND_ARTICLE, BRAND_FULL_NAME, BRAND_FACEBOOK_GROUP), true);
    this->addView(links);
}