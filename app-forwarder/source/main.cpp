#include <filesystem>
#include <string>
#include <fstream>

#include <switch.h>

int removeDir(const char* path)
{
    Result ret = 0;
    FsFileSystem *fs = fsdevGetDeviceFileSystem("sdmc");
    if (R_FAILED(ret = fsFsDeleteDirectoryRecursively(fs, path))) {
        return ret;
    }
    return 0;
}

std::string readConfFile(const std::string& section)
{
    std::ifstream file("forwarder.conf");
    std::string line;

    if (file.is_open())
    {
        size_t pos_char;
        while (std::getline(file, line)) {
            if ((pos_char = line.find(section + "=", 0)) != std::string::npos)
            {
                line = line.substr((pos_char + section.size() + 1), line.size() - (section.size() + 1));
                break;
            }
            else
                line = "";
        }
        file.close();
    }
    return line;
}

void writeLog(std::string line)
{
    std::ofstream logFile;
    logFile.open("LOG_FORWARDAR.txt", std::ofstream::out | std::ofstream::app);
    if (logFile.is_open()) {
        logFile << line << std::endl;
    }
    logFile.close();
}

int main(int argc, char* argv[])
{
    std::string PATH           = readConfFile("PATH");
    std::string FULL_PATH      = readConfFile("FULL_PATH");
    std::string CONFIG_PATH    = readConfFile("CONFIG_PATH");
    std::string PREFIX         = readConfFile("PREFIX");
    std::string FORWARDER_PATH = readConfFile("FORWARDER_PATH");
    std::string CONFIG_SWITCH  = readConfFile("CONFIG_SWITCH");
    std::string HIDDEN_FILE    = readConfFile("HIDDEN_FILE");

    std::filesystem::create_directory(PATH.c_str());
    for (const auto & entry : std::filesystem::directory_iterator(PATH.c_str())){
        if(entry.path().string().find(PREFIX.c_str()) != std::string::npos) {
            std::filesystem::remove(entry.path().string());
            std::filesystem::remove(entry.path().string() + ".star");
        }
    }
    std::filesystem::remove(HIDDEN_FILE.c_str());

    std::filesystem::remove("forwarder.conf");

    if(std::filesystem::exists(CONFIG_PATH.c_str())){
        std::filesystem::create_directory(PATH.c_str());
        std::filesystem::remove(FULL_PATH.c_str());
        std::filesystem::rename(CONFIG_PATH.c_str(), FULL_PATH.c_str());
        removeDir(CONFIG_SWITCH.c_str());
    }
    std::filesystem::remove(FORWARDER_PATH.c_str());
    envSetNextLoad(FULL_PATH.c_str(), ("\"" + std::string(FULL_PATH.c_str()) + "\"").c_str());
    return 0;
}
