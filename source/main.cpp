#include <switch.h>

#include <borealis.hpp>
#include <filesystem>
#include <json.hpp>

#include "constants.hpp"
#include "current_cfw.hpp"
#include "fs.hpp"
#include "main_frame.hpp"
#include "warning_page.hpp"
#include "beta_page.hpp"
#include "MOTD_page.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

CFW CurrentCfw::running_cfw;

void showScreen(const bool& existHiddenFile)
{
    std::string sTmp = APP_VERSION;
    bool beta = (sTmp.find("b") != std::string::npos);

    bool bAlwaysShowMOTD, bWasMOTDDisplayed;
    sTmp = util::getMOTD(bAlwaysShowMOTD);
    bWasMOTDDisplayed = util::wasMOTDAlreadyDisplayed();

    if (existHiddenFile) {
        if (beta)
            brls::Application::pushView(new BetaPage(((bAlwaysShowMOTD) || (!bWasMOTDDisplayed)))); //TRUE = SHOW MOTD / FALSE = DOES NOT SHOW MOTD
        else
        {
            if ((bAlwaysShowMOTD) || (!bWasMOTDDisplayed))
                brls::Application::pushView(new MOTDPage());
            else
                brls::Application::pushView(new MainFrame());
        }
    }
    else
    {
//        if ((bAlwaysShowMOTD) || (!bWasMOTDDisplayed))
//            brls::Application::pushView(new WarningPage("menus/main/launch_warning"_i18n, true)); //1 BOOL = MOTD - 2 BOOL BETA
//        else
//            brls::Application::pushView(new WarningPage("menus/main/launch_warning"_i18n, false)); //1 BOOL = MOTD - 2 BOOL BETA
        brls::Application::pushView(new WarningPage("menus/main/launch_warning"_i18n, ((bAlwaysShowMOTD) || (!bWasMOTDDisplayed)), beta)); //1 BOOL = MOTD - 2 BOOL BETA
    }
}
int main(int argc, char* argv[])
{
    // Init the app
    if (!brls::Application::init(APP_TITLE)) {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    nlohmann::json languageFile = fs::parseJsonFile(fmt::format(LANGUAGE_JSON, BASE_FOLDER_NAME));
    if (languageFile.find("language") != languageFile.end())
        i18n::loadTranslations(languageFile["language"]);
    else
        i18n::loadTranslations();

        //appletInitializeGamePlayRecording();

        // Setup verbose logging on PC
#ifndef __SWITCH__
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
#endif

    setsysInitialize();
    plInitialize(PlServiceType_User);
    nsInitialize();
    socketInitializeDefault();
    nxlinkStdio();
    pmdmntInitialize();
    pminfoInitialize();
    splInitialize();
    romfsInit();

    CurrentCfw::running_cfw = CurrentCfw::getCFW();

    fs::createTree(fmt::format(CONFIG_PATH, BASE_FOLDER_NAME));

    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
    brls::Logger::debug("Start");

    showScreen(std::filesystem::exists(fmt::format(HIDDEN_APG_FILE, BASE_FOLDER_NAME, BASE_FOLDER_NAME)));

    while (brls::Application::mainLoop());

    romfsExit();
    splExit();
    pminfoExit();
    pmdmntExit();
    socketExit();
    nsExit();
    setsysExit();
    plExit();
    return EXIT_SUCCESS;
}
