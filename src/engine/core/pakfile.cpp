#include "pakfile.hpp"
#include "logger.hpp"

#include <fstream>
#include <iostream>

namespace Engine::Core {
    PakFile::~PakFile() {
        LOG_DEBUG("Destryoing pakfile");
        mFiles.clear();
    }

    int PakFile::Load(const std::string& filePath) {
        mFiles.clear();

        LOG_INFO("Loading pak bundle " + filePath);
        std::ifstream in(filePath, std::ios::binary);
        if(!in) return -1;

        PakHeader header;
        in.read(header.format, 3);
        if(strncmp(header.format, "PAK", 3) != 0) {
            throw std::runtime_error("Invalid .pak file : bad format");
        }
        uint32_t version;
        uint32_t numFiles;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));
        in.read(reinterpret_cast<char*>(&numFiles), sizeof(numFiles));

        while(in.peek() != EOF) {
            uint32_t nameLen;
            in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            if(in.eof()) break;

            std::string name(nameLen, '\0');
            in.read(&name[0], nameLen);

            uint32_t dataLen;
            in.read(reinterpret_cast<char*>(&dataLen), sizeof(dataLen));
            std::vector<char> data(dataLen);
            in.read(data.data(), dataLen);

            mFiles[name] = std::move(data);
        }
    }

    std::vector<char> PakFile::GetBinaryFile(const std::string& name) const {
        auto it = mFiles.find(name);
        if (it != mFiles.end()) return it->second;

        LOG_ERROR(std::string("File not found " + name));
        LOG_ERROR("Possible files for this pak are:");
        for(auto it: mFiles) {
            LOG_ERROR(it.first);
        }
        return {};
    }

    std::string PakFile::GetTextFile(const std::string& name) const {
        auto bin = GetBinaryFile(name);
        return std::string(bin.begin(), bin.end());
    }
}