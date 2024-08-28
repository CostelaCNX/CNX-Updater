#pragma once
/*
CHANGE ONLY THESE CONSTANTS
*/
constexpr const char BRAND_ARTICLE[] = "";
constexpr const char BRAND_FULL_NAME[] = "";
constexpr const char BRAND_FACEBOOK_GROUP[] = "";
constexpr const char APP_FULL_NAME[] = "";
constexpr const char APP_SHORT_NAME[] = "";
constexpr const char BASE_FOLDER_NAME[] = "";
constexpr const char GITHUB_USER[] = "";
constexpr const bool SHOW_GNX = true;

/*
DO NOT CHANGE ONLY THESE CONSTANTS UNLESS YOU KNOW WHAT YOU'RE DOING
*/

constexpr const bool DEBUG = false;

constexpr const char CFW_ROOT_PATH[] = "/apgtmppackfolder";
constexpr const char ROOT_PATH[] = "/";
constexpr const char DOWNLOAD_PATH[] = "/config/{}-updater/";
constexpr const char CONFIG_PATH[] = "/config/{}-updater/";

constexpr const char RCM_PAYLOAD_PATH[] = "romfs:/app_rcm.bin";
constexpr const char MARIKO_PAYLOAD_PATH[] = "/payload.bin";
constexpr const char MARIKO_PAYLOAD_PATH_TEMP[] = "/payload.bin.apg";

constexpr const char APP_URL[] = "https://github.com/{}/{}-updater/releases/latest/download/{}-updater.zip";
constexpr const char APP_INFO[] = "https://api.github.com/repos/{}/{}-updater/releases/latest";
constexpr const char APP_FILENAME[] = "/config/{}-updater/app.zip";

constexpr const char NXLINKS_URL[] = "https://raw.githubusercontent.com/gamemoddesignbr/nx-links/master/nx-links-v204.json";

constexpr const char FIRMWARE_FILENAME[] = "/config/{}-updater/firmware.zip";
constexpr const char FIRMWARE_PATH[] = "/firmware/";

constexpr const char AMS_FILENAME[] = "/config/{}-updater/ams.zip";

constexpr const char FILES_IGNORE[] = "/config/{}-updater/preserve.txt";
constexpr const char INTERNET_JSON[] = "/config/{}-updater/internet.json";

constexpr const char AMS_CONTENTS[] = "/atmosphere/contents/";
constexpr const char REINX_CONTENTS[] = "/ReiNX/contents/";
constexpr const char SXOS_TITLES[] = "/sxos/titles/";
constexpr const char AMS_PATH[] = "/atmosphere/";
constexpr const char SXOS_PATH[] = "/sxos/";
constexpr const char REINX_PATH[] = "/ReiNX/";
constexpr const char CONTENTS_PATH[] = "contents/";
constexpr const char TITLES_PATH[] = "titles/";

constexpr const char UPDATE_BIN_PATH[] = "/bootloader/update.bin";
constexpr const char REBOOT_PAYLOAD_PATH[] = "/atmosphere/reboot_payload.bin";

constexpr const char AMS_ZIP_PATH[] = "/config/{}-updater/ams.zip";
constexpr const char APP_ZIP_PATH[] = "/config/{}-updater/app.zip";
constexpr const char FW_ZIP_PATH[] = "/config/{}-updater/firmware.zip";
constexpr const char TRANSLATIONS_ZIP_PATH[] = "/config/{}-updater/translations.zip";
constexpr const char MODIFICATIONS_ZIP_PATH[] = "/config/{}-updater/modifications.zip";
constexpr const char AMS_DIRECTORY_PATH[] = "/config/{}-updater/atmosphere/";
constexpr const char SEPT_DIRECTORY_PATH[] = "/config/{}-updater/sept/";
constexpr const char FW_DIRECTORY_PATH[] = "/firmware/";

constexpr const char COPY_FILES_TXT[] = "/config/{}-updater/copy_files.txt";
constexpr const char LANGUAGE_JSON[] = "/config/{}-updater/language.json";

constexpr const char ROMFS_FORWARDER[] = "romfs:/app-forwarder.nro";
constexpr const char FORWARDER_PATH[] = "/config/{}-updater/app-forwarder.nro";
constexpr const char FORWARDER_CONF[] = "/config/{}-updater/forwarder.conf";

constexpr const char DAYBREAK_PATH[] = "/switch/daybreak.nro";

constexpr const char HIDDEN_APG_FILE[] = "/config/{}-updater/.{}-updater";

constexpr const char APG_STAR_FILE[] = "/switch/{}-updater/.{}-updater.nro.star";

constexpr const char LOG_FILE[] = "/config/{}-updater/log.txt";

constexpr const char PACK_FILE[] = "/bootloader/hekate_ipl.ini";

constexpr const char AMS_EMUNAND_FILE[] = "/emuMMC/emummc.ini";

constexpr const char MOTD_KEY[] = "{}-MOTD";

constexpr const int LISTITEM_HEIGHT = 50;

constexpr const int MAX_FETCH_LINKS = 15;

constexpr const char CLEAN_INSTALL_FLAG[] = "/cleaninstall.flag";

enum class contentType
{
    fw,
    app,
    ams_cfw,
    translations,
    modifications
};

constexpr std::string_view contentTypeNames[5]{"firmwares", "app", "cfws", "translations", "modifications"};
constexpr std::string_view contentTypeFullNames[5]{"o firmware", "o homebrew", "o CFW", "a tradução", "a modificação"};

enum class DialogType
{
    updating,
    warning,
    error,
    beta
};

constexpr std::string_view ROMFSIconFile[4]{"romfs:/updating.png", "romfs:/warning.png", "romfs:/error.png", "romfs:/beta.png"};

enum class CFW
{
    rnx,
    sxos,
    ams,
};
