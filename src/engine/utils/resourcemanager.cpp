#include "resourcemanager.hpp"

#include <iostream>
#include <unordered_map>

#include <engine/core/pakfile.hpp>

namespace Engine::Utils {
    std::unordered_map<std::string, Core::PakFile*> ResourceManager::mPaks;
    std::unordered_map<std::string, Graphics::Texture*> ResourceManager::mTextures;
    std::unordered_map<std::string, Graphics::Shader*> ResourceManager::mShaders;
    std::unordered_map<std::string, UI::Font*> ResourceManager::mFonts;
    std::unordered_map<std::string, Audio::Sound*> ResourceManager::mSounds;

    void ResourceManager::Clear() {
        auto deleteAndClear = [](auto& map) {
            for (auto& [_, ptr] : map) {
                delete ptr;
            }
            map.clear();
        };

        deleteAndClear(mPaks);
        deleteAndClear(mTextures);
        deleteAndClear(mShaders);
        deleteAndClear(mFonts);
        deleteAndClear(mSounds);
    }

    Core::PakFile& ResourceManager::LoadPak(const std::string& pakPath, const std::string& name) {
        std::string pakName = name.size() ? name : pakPath;

        if(mPaks.find(pakName) == mPaks.end()) {
            mPaks[pakName] = new Core::PakFile();
            mPaks[pakName]->Load(pakPath);
        }

        return *(mPaks[pakName]);
    }

    void ResourceManager::FreePak(const std::string& pakName) {
        if(mPaks.find(pakName) != mPaks.end())
            mPaks.erase(pakName);
    }

    Core::Buffer ResourceManager::GetBinaryFileFromPak(const std::string& pakName, const std::string& fileName) {
        if(mPaks.find(pakName) == mPaks.end()) {
            LoadPak(pakName);
        }

        return mPaks[pakName]->GetBinaryFile(fileName);
    }

    std::string ResourceManager::GetTextFileFromPak(const std::string& pakName, const std::string& fileName) {
        if(mPaks.find(pakName) == mPaks.end()) {
            LoadPak(pakName);
        }

        return mPaks[pakName]->GetTextFile(fileName);
    }

    Graphics::Texture* ResourceManager::GetTexture(const std::string& pakAndFileName) {
        auto cacheIter = mTextures.find(pakAndFileName);
        if(cacheIter != mTextures.end()) {
            return cacheIter->second;
        }

        // Split le pak name (pakname:filename.extension)
        auto delimiterPos = pakAndFileName.find(':');
        if(delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid resource path format. Expected 'pak:filename.extension'");
        }

        std::string pakName = pakAndFileName.substr(0, delimiterPos);
        std::string fileName = pakAndFileName.substr(delimiterPos + 1);

        Core::Buffer buffer = GetBinaryFileFromPak(pakName, fileName);
        auto* tex = new Graphics::Texture(buffer.data(), buffer.size());

        mTextures[pakAndFileName] = tex;
        return tex;
    }

    Graphics::Shader* ResourceManager::GetShader(const std::string& pakAndFileName) {
        auto cacheIter = mShaders.find(pakAndFileName);
        if(cacheIter != mShaders.end()) {
            return cacheIter->second;
        }

        // Split le pak name (pakname:filename.extension)
        auto delimiterPos = pakAndFileName.find(':');
        if(delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid resource path format. Expected 'pak:filename.extension'");
        }

        std::string pakName = pakAndFileName.substr(0, delimiterPos);
        std::string fileName = pakAndFileName.substr(delimiterPos + 1);

        std::string vertPath = fileName + ".vert";
        std::string fragPath = fileName + ".frag";

        std::string vertSource = GetTextFileFromPak(pakName, vertPath);
        std::string fragSource = GetTextFileFromPak(pakName, fragPath);
        auto* shader = new Graphics::Shader(vertSource.c_str(), fragSource.c_str());

        mShaders[pakAndFileName] = shader;
        return shader;
    }

    UI::Font* ResourceManager::GetFont(const std::string& pakAndFileName, int fontSize) {
        auto cacheIter = mFonts.find(pakAndFileName);
        if(cacheIter != mFonts.end()) {
            return cacheIter->second;
        }

        // Split le pak name (pakname:filename.extension)
        auto delimiterPos = pakAndFileName.find(':');
        if(delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid resource path format. Expected 'pak:filename.extension'");
        }

        std::string pakName = pakAndFileName.substr(0, delimiterPos);
        std::string fileName = pakAndFileName.substr(delimiterPos + 1);

        Core::Buffer tmpBuffer = GetBinaryFileFromPak(pakName, fileName);
        auto* font = new UI::Font(tmpBuffer.data(), tmpBuffer.size(), fontSize);

        mFonts[pakAndFileName] = font;
        return font;
    }

    Audio::Sound* ResourceManager::GetSound(const std::string& pakAndFileName, Audio::SoundType soundType) {
        auto cacheIter = mSounds.find(pakAndFileName);
        if(cacheIter != mSounds.end()) {
            return cacheIter->second;
        }

        // Split le pak name (pakname:filename.extension)
        auto delimiterPos = pakAndFileName.find(':');
        if(delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid resource path format. Expected 'pak:filename.extension'");
        }

        std::string pakName = pakAndFileName.substr(0, delimiterPos);
        std::string fileName = pakAndFileName.substr(delimiterPos + 1);

        Core::Buffer tmpBuffer = GetBinaryFileFromPak(pakName, fileName);
        auto* sound = Audio::AudioManager::LoadSoundFromMemory(tmpBuffer.data(), tmpBuffer.size());

        mSounds[pakAndFileName] = sound;
        return sound;
    }
}