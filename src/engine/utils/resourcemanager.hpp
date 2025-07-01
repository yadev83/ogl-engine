/**
 * @file resourcemanager.hpp
 * @brief 
 */
#pragma once

#include <iostream>
#include <unordered_map>

#include <engine/core/pakfile.hpp>
#include <engine/graphics/texture.hpp>
#include <engine/graphics/shader.hpp>
#include <engine/ui/font.hpp>
#include <engine/audio/audiomanager.hpp>
#include <engine/audio/sound.hpp>

namespace Engine::Utils {
    /**
     * @brief La classe resource manager permet la gestion de fichiers .pak de façon statique pour donner accès de manière globale aux assets
     * 
     */
    class ResourceManager {
        private:
            static std::unordered_map<std::string, Core::PakFile*> mPaks;

            static std::unordered_map<std::string, Graphics::Texture*> mTextures;
            static std::unordered_map<std::string, Graphics::Shader*> mShaders;
            static std::unordered_map<std::string, UI::Font*> mFonts;
            static std::unordered_map<std::string, Audio::Sound*> mSounds;
            
            ResourceManager() = default;
            ~ResourceManager() = default;
            
        public:
            /**
             * @brief Nettoie les registres du gestionnaire de ressources
             * 
             * Est appelée au moment de la fermeture de l'application
             */
            static void Clear();

            /**
             * @brief Charge un fichier .pak en mémoire
             * 
             * @param pakPath 
             * @param name Un nom custom pour accéder aux données plus tard
             * @return Core::PakFile 
             */
            static Core::PakFile& LoadPak(const std::string& pakPath, const std::string& name = "");
            /**
             * @brief Libère le fichier pak chargé à l'endroit donné
             * 
             * @param pakName
             */
            static void FreePak(const std::string& pakName);

            /**
             * @brief Renvoie le fichier binaire contenu dans le fichier .pak
             * 
             * @param pakName Le nom du pak à lire
             * @param fileName le nom du fichier à trouver dans le pak
             * @return Core::Buffer 
             */
            static Core::Buffer GetBinaryFileFromPak(const std::string& pakName, const std::string& fileName);
            /**
             * @brief Renvoie le fichier binaire contenu dans le fichier .pak
             * 
             * @param pakName Le nom du pak à lire
             * @param fileName le nom du fichier à trouver dans le pak
             * @return std::string 
             */
            static std::string GetTextFileFromPak(const std::string& pakName, const std::string& fileName);

            /** UTILITAIRES QUI RENVOIENT DIRECTEMENT DES OBJETS MOTEUR PLUTÖT QUE DES BUFFERS/TEXTES */
            /**
             * @brief Renvoie la texture demandée, et chargée depuis le fichier pak donné
             * 
             * @param pakAndFileName Le chemin de la texture au format : "pak:filename.extension"
             * @return Graphics::Texture* Une texture unique, chargée la première fois, et get depuis le cache sinon.
             */
            static Graphics::Texture* GetTexture(const std::string& pakAndFileName);

            /**
             * @brief Renvoie un shader précompilé
             * 
             * @param pakAndFileName Le chemin vers le shader au format : "pak:filename" => l'extension est automatique (.vert/.frag)
             * @return Graphics::Shader* 
             */
            static Graphics::Shader* GetShader(const std::string& pakAndFileName);
            /**
             * @brief Renvoie une font préchargée de taille donnée
             * 
             * @param pakAndFileName Format vers le fichier : "pak:filename.extension"
             * @param fontSize La taille de la font à charger (possiblement plusieurs fontes du même nom avec des tailles différentes en mémoire)
             * @return UI::Font* 
             */
            static UI::Font* GetFont(const std::string& pakAndFileName, int fontSize = 48);
            /**
             * @brief Renvoie un fichier audio préchargé
             * 
             * @param pakAndFileName Format attendu : "pak:filename.extension"
             * @param soundType Le type de son (SFX/BGM) chargé
             * @return Audio::Sound* 
             */
            static Audio::Sound* GetSound(const std::string& pakAndFileName, Audio::SoundType soundType = Audio::SoundType::SFX);
    };
}