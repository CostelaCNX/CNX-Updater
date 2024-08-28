#pragma once

constexpr int ON = 1;
constexpr int OFF = 0;

#include <json.hpp>

namespace download {

    long downloadFile(const std::string& url, std::vector<std::uint8_t>& res, const std::string& output = "", int api = OFF);
    long downloadFile(const std::string& url, const std::string& output = "", int api = OFF);
    long downloadFileFaster(const nlohmann::ordered_json& json, std::vector<std::uint8_t>& res, const std::string& output = "", int api = OFF);
    long downloadFileFaster(const nlohmann::ordered_json& json, const std::string& output = "", int api = OFF);
    std::vector<std::pair<std::string, std::string>> getLinks(const std::string& url);
    std::vector<std::pair<std::string, std::string>> getLinksFromJson(const nlohmann::ordered_json& json_object);
    std::string fetchTitle(const std::string& url);
    long downloadPage(const std::string& url, std::string& res, const std::vector<std::string>& headers = {}, const std::string& body = "");
    long getRequest(const std::string& url, nlohmann::ordered_json& res, const std::vector<std::string>& headers = {}, const std::string& body = "");
    std::vector<std::pair<std::string, std::string>> getLinksFromGitHubReleases(std::string& url, int max);
}  // namespace download