#pragma once
#include <sstream>

class TextHelpers {
 public:
  static void tokenize(std::string const &str, std::string delim, std::vector<std::string> &out) {
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
      end = str.find(delim, start);
      out.push_back(str.substr(start, end - start));
    }
  }

  static std::string filter(std::string str) {
    std::string output;
    output.reserve(str.size());  // optional, avoids buffer reallocations in the loop
    for (size_t i = 0; i < str.size(); ++i) {
      if (i == 0 && str[i] == ' ')
        continue;
      if (i < str.size() - 3 && str[i] == '\\' && str[i + 1] == 'x' && str[i + 2] == 'a') {
        // replace \xa with space
        output += ' ';
        i += 3;
        continue;
      }
      if (i == str.size() - 1 && str[i] == '}')
        return output;
      if (str[i] != '[' && str[i] != ']' && str[i] != '\'' && str[i] != '"')
        output += str[i];
    }
    return output;
  }

  static std::vector<std::shared_ptr<MenuTitleSource>> parseJsonArray(std::string state, std::string entityId) {
    StaticJsonDocument<3072> doc;
    DeserializationError err = deserializeJson(doc, state);
    std::vector<std::shared_ptr<MenuTitleSource>> sources;
    if (err) {
      ESP_LOGE("JSON", "deserializeJson() failed: ");
      ESP_LOGE("JSON", err.c_str());
      return sources;
    }
    JsonArray array = doc.as<JsonArray>();
    for (JsonVariant v : array) {
      std::string sourceName = v.as<std::string>();
      ESP_LOGD("JSON", "new JSON array value %s %s", sourceName.c_str(), entityId.c_str());
      auto newsource = std::make_shared<MenuTitleSource>(sourceName, entityId, NoMenuTitleLeftIcon,
                                                         NoMenuTitleRightIcon, SourceRemotePlayerSourceType);
      sources.push_back(newsource);
    }
    return sources;
  }

  static std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length();  // Handles case where 'to' is a substring of 'from'
    }
    return str;
  }

  static std::vector<std::shared_ptr<MenuTitleSource>> parseJsonSource(std::string state, std::string nameKey,
                                                                       std::string valueKey) {
    StaticJsonDocument<3072> doc;
    DeserializationError err = deserializeJson(doc, state);
    std::vector<std::shared_ptr<MenuTitleSource>> sources;
    if (err) {
      ESP_LOGE("JSON", "deserializeJson() failed: ");
      ESP_LOGE("JSON", err.c_str());
      return sources;
    }
    JsonArray array = doc.as<JsonArray>();
    for (JsonVariant v : array) {
      std::string playlistName = v[nameKey].as<std::string>();
      std::string playlistId = v[valueKey].as<std::string>();
      ESP_LOGD("JSON", "new JSON object value %s %s", playlistId.c_str(), playlistName.c_str());
      auto newsource = std::make_shared<MenuTitleSource>(playlistName, playlistId, NoMenuTitleLeftIcon,
                                                         NoMenuTitleRightIcon, MusicRemotePlayerSourceType);
      sources.push_back(newsource);
    }
    return sources;
  }

  static std::vector<std::shared_ptr<MenuTitleSource>> parseJsonKeyValue(std::string state) {
    StaticJsonDocument<3072> doc;
    DeserializationError err = deserializeJson(doc, state);
    std::vector<std::shared_ptr<MenuTitleSource>> sources;
    if (err) {
      ESP_LOGE("JSON", "deserializeJson() failed: ");
      ESP_LOGE("JSON", err.c_str());
      return sources;
    }
    JsonObject array = doc.as<JsonObject>();
    for (JsonPair v : array) {
      std::string playlistName(v.value().as<std::string>());
      std::string playlistId(v.key().c_str());
      ESP_LOGD("group", "new JSON key value %s %s", playlistId.c_str(), playlistName.c_str());
      auto newsource = std::make_shared<MenuTitleSource>(playlistName, playlistId, NoMenuTitleLeftIcon,
                                                         NoMenuTitleRightIcon, FavoriteItemIDRemotePlayerSourceType);
      sources.push_back(newsource);
    }
    return sources;
  }

  static int extractFirstNumber(std::string input) {
    // Remove the opening and closing parentheses
    input = input.substr(1, input.size() - 2);
    // Split the string on the comma
    std::istringstream iss(input);
    std::string first, second;
    std::getline(iss, first, ',');
    // Convert the first part to a int
    int num = std::stod(first);
    return num;
  }

 private:
  TextHelpers() {}
};
