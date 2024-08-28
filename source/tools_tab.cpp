#include "tools_tab.hpp"

#include <filesystem>
#include <fstream>

#include "changelog_page.hpp"
#include "icons_page.hpp"
#include "confirm_page.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "net_page.hpp"
#include "utils.hpp"
#include "worker_page.hpp"
#include "download.hpp"

#include <chrono>

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

namespace {
    constexpr const char AppVersion[] = APP_VERSION;
}

ToolsTab::ToolsTab(const std::string& tag, bool erista) : brls::List()
{
    brls::ListItem* netSettings = new brls::ListItem("menus/tools/internet_settings"_i18n);
    netSettings->getClickEvent()->subscribe([](brls::View* view) {
        brls::PopupFrame::open("menus/tools/internet_settings"_i18n, new NetPage(), "", "");
    });
    netSettings->setHeight(LISTITEM_HEIGHT);

    brls::ListItem* browser = new brls::ListItem("menus/tools/browser"_i18n);
    browser->getClickEvent()->subscribe([](brls::View* view) {
        std::string url;
        if (brls::Swkbd::openForText([&url](std::string text) { url = text; }, "cheatslips.com e-mail", "", 64, "https://duckduckgo.com", 0, "Submit", "https://website.tld")) {
            std::string error = "";
            int at = appletGetAppletType();
            if (at == AppletType_Application) {    // Running as a title
                WebCommonConfig conf;
                WebCommonReply out;
                Result rc = webPageCreate(&conf, url.c_str());
                if (R_FAILED(rc))
                    error += "\uE016 Error starting Browser\n\uE016 Lookup error code for more info " + rc;
                webConfigSetJsExtension(&conf, true);
                webConfigSetPageCache(&conf, true);
                webConfigSetBootLoadingIcon(&conf, true);
                webConfigSetWhitelist(&conf, ".*");
                rc = webConfigShow(&conf, &out);
                if (R_FAILED(rc))
                    error += "\uE016 Error starting Browser\n\uE016 Lookup error code for more info " + rc;
            }
            else {  // Running under applet
                error += "\uE016 Running in applet mode/through a forwarder.\n\uE016 Please launch hbmenu by holding [R] on a game";
            }
            if (!error.empty()) {
                util::showDialogBoxInfo(error);
            }
        }
    });
    browser->setHeight(LISTITEM_HEIGHT);

    brls::ListItem* cleanUp = new brls::ListItem("menus/tools/clean_up"_i18n);
    cleanUp->getClickEvent()->subscribe([](brls::View* view) {
        util::cleanFiles();
        util::showDialogBoxInfo("menus/common/all_done"_i18n);
    });
    cleanUp->setHeight(LISTITEM_HEIGHT);

    brls::ListItem* motd = new brls::ListItem("menus/tools/motd_label"_i18n);

    bool bAlwaysShow;
	std::string sMOTD = util::getMOTD(bAlwaysShow);
    motd->getClickEvent()->subscribe([sMOTD](brls::View* view) {
      util::showDialogBoxInfo(sMOTD);
    });
    motd->setHeight(LISTITEM_HEIGHT);

    brls::ListItem* changelog = new brls::ListItem(fmt::format("{} no homebrew", "menus/changelog/changelog"_i18n));
    changelog->getClickEvent()->subscribe([](brls::View* view) {
        brls::PopupFrame::open(fmt::format("{} no homebrew", "menus/changelog/changelog"_i18n), new ChangelogPage(), "", "");
    });
    changelog->setHeight(LISTITEM_HEIGHT);

    this->addView(netSettings);
    this->addView(browser);
    this->addView(cleanUp);
    if (sMOTD != "")
	    this->addView(motd);
    this->addView(changelog);

    if (DEBUG)
    {
        brls::ListItem* icons = new brls::ListItem("\uE150 Icones");
        icons->getClickEvent()->subscribe([](brls::View* view) {
            brls::PopupFrame::open("Icones", new IconsPage(), "", "");
        });
        icons->setHeight(LISTITEM_HEIGHT);
        this->addView(icons);

        brls::ListItem* forceCleanInstall = new brls::ListItem("\uE150 Forçar arquivo de instalação limpa - NAO USE!");
        forceCleanInstall->getClickEvent()->subscribe([](brls::View* view) {
            util::createCleanInstallFile();
			util::showDialogBoxInfo("Agora use a opção 'Reiniciar no Payload RCM' para testar a 'Instalação Limpa'.");
        });
        forceCleanInstall->setHeight(LISTITEM_HEIGHT);
        this->addView(forceCleanInstall);

        brls::ListItem* payloadRCM = new brls::ListItem("\uE150 Reiniciar no Payload RCM - NAO USE!");
        payloadRCM->getClickEvent()->subscribe([](brls::View* view) {
            if (util::isErista()) {
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
        });
        payloadRCM->setHeight(LISTITEM_HEIGHT);
        this->addView(payloadRCM);

        brls::ListItem* test = new brls::ListItem("\uE150 Versão?");
        test->getClickEvent()->subscribe([](brls::View* view) {

			std::string src = APP_VERSION;
            std::string trg = "";

			for (char c : src) {
				if (std::isdigit(c)) trg += c;
			}
			
            util::showDialogBoxInfo(fmt::format("Versão por extenso: {}\nVersão calculada: {}", src, trg));
        });
        test->setHeight(LISTITEM_HEIGHT);
        this->addView(test);

        brls::ListItem* json = new brls::ListItem("\uE150 JSON");
        json->getClickEvent()->subscribe([](brls::View* view) {

			nlohmann::ordered_json json;
			download::getRequest("https://raw.githubusercontent.com/gamemoddesignbr/gmpack/main/latest/pegascape/json_file2.json", json, {"accept: application/vnd.github.v3+json"});

/*
			std::vector<std::string> filesList{};
			std::vector<std::string> foldersList{};
			json.at("files").get_to(filesList);
			json.at("dirs").get_to(foldersList);
			std::string tmp = "";
			for (const auto& item : filesList)
			{
				tmp = fmt::format("{}{}\n", tmp, item);
			}
            util::showDialogBoxInfo(fmt::format("Arquivos:\n{}", tmp));
*/

			brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
			stagedFrame->setTitle("TESTE TITLE");
			stagedFrame->addStage(new ConfirmPage(stagedFrame, "CONFIRMAR"));
			stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [json]() { util::downloadArchive(json, contentType::ams_cfw); }));
			stagedFrame->addStage(new ConfirmPage(stagedFrame, "FINAL"));
			brls::Application::pushView(stagedFrame);




        });
        json->setHeight(LISTITEM_HEIGHT);
        this->addView(json);
    }
}
