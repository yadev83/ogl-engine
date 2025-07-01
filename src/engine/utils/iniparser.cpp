#include "iniparser.hpp"
#include "../core/logger.hpp"

namespace Engine::Utils {
    std::string IniParser::Trim(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;

        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;

        return s.substr(start, end - start);
    }

    int IniParser::Load(const std::string& filepath) {
        mData.clear();

        std::ifstream file(filepath);
        if(!file.is_open()) {
            LOG_ERROR("Could not parse ini file: " + filepath);
            return -1;
        }

        std::string line;
        std::string currentSection;

        while(std::getline(file, line)) {
            line = Trim(line);
            if(line.empty() || line[0] == ';' || line[0] == '#') continue;

            if(line.front() == '[' && line.back() == ']') {
                // New section
                currentSection = line.substr(1, line.size() - 2);
                currentSection = Trim(currentSection);
            } else {
                // Key=Value inside of currentSection
                auto eqPos = line.find('=');
                if(eqPos == std::string::npos) continue; // Skip if no "=" sign

                std::string key = Trim(line.substr(0, eqPos));
                std::string value = Trim(line.substr(eqPos + 1));

                if(!currentSection.empty() && !key.empty()) {
                    mData[currentSection][key] = value;
                }
            }
        }

        return true;
    }

    std::string IniParser::GetString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
        auto sectionIt = mData.find(section);
        if(sectionIt != mData.end()) {
            auto dataIt = sectionIt->second.find(key);
            if(dataIt != sectionIt->second.end())
                return dataIt->second;
        }

        return defaultValue;
    }

    int IniParser::GetInt(const std::string& section, const std::string& key, int defaultValue) const {
        std::string val = GetString(section, key);

        try {
            return std::stoi(val);
        } catch(...) {
            return defaultValue;
        }
    }

    float IniParser::GetFloat(const std::string& section, const std::string& key, float defaultValue) const {
        std::string val = GetString(section, key);

        try {
            return std::stof(val);
        } catch(...) {
            return defaultValue;
        }
    }

    bool IniParser::GetBool(const std::string& section, const std::string& key, bool defaultValue) const {
        std::string val = GetString(section, key);
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);

        if (val == "true" || val == "1" || val == "yes")
            return true;
        if (val == "false" || val == "0" || val == "no")
            return false;
            
        return defaultValue;
    }

    bool IniParser::HasSection(const std::string& section) const {
        return mData.find(section) != mData.end();
    }

    bool IniParser::HasKey(const std::string& section, const std::string& key) const {
        auto sectionIt = mData.find(section);
        if (sectionIt == mData.end()) return false;
        return sectionIt->second.find(key) != sectionIt->second.end();
    }
}