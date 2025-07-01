/**
 * @file audiomanager.hpp
 * @brief Wrapper statique autour de l'api OpenAL pour charger, et lire des sons.
 */
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <AL/al.h>
#include <AL/alc.h>
#include "sound.hpp"

namespace Engine::Audio {
    /**
     * @brief Classe statique qui gère tous les sons et la lecture audio dans le moteur
     * Il faut juste appeler AudioManager::Init() une fois avant de pouvoir s'en servir (c'est fait dans la classe App) 
     */
    class AudioManager {
        private:
            /** @brief Périphérique audio détecté et utilisé par OpenAL (pointeur défini durant Init()) */
            static ALCdevice *device;
            /** @brief Le contexte OpenAL pour jouer des sons (pointeur défini durant Init()) */
            static ALCcontext *context;

            /**
             * @brief Charge un son au format WAV depuis un buffer mémoire
             * 
             * @param buffer La donnée originale du fichier son
             * @param size La taille du buffer de donnée à interpréter
             * @return ALuint L'identifiant du buffer chargé par OpenAL
             */
            static ALuint LoadWAVFromMemory(const char *buffer, size_t size);
            /**
             * @brief Charge un fichier WAV depuis un path donné
             * 
             * @param filePath Le chemin vers le fichier à lire
             * @return ALuint L'identifiant du buffer chargé par OpenAL
             */
            static ALuint LoadWAV(const std::string& filePath);

        public:
            // Suppression du Constructeur / Destructeur pour éviter les erreurs d'inatention à l'usage (full static class)
            AudioManager() = delete;
            ~AudioManager() = delete;
            
            /** @brief Fonction qui initialiser la classe AudioManager en créant le contexte et le device OpenAL */
            static void Init();
            /** @brief Ferme les contextes/devices OpenAL */
            static void Shutdown();

            /**
             * @brief Charge un son avec l'API OpenAL et renvoie un pointeur vers un objet Sound
             * 
             * @param filepath Le chemin du fichier à lire
             * @param fileType Le type de fichier (WAV par défaut)
             * @param type Le type de son à charger (SFX par défaut)
             * @return Sound* Pointeur vers le son chargé
             */
            static Sound *LoadSound(const std::string& filepath, SoundFileType fileType = WAV, SoundType type = SFX);
            /**
             * @brief Charge un son depuis un buffer mémoire plutôt qu'un path
             * 
             * @param buffer Le buffer contenant la donnée audio binaire
             * @param size La taille du buffer à lire
             * @param fileType Le type de fichier à lire (WAV par défaut) 
             * @param type Le type de son à charger (SFX par défaut)
             * @return Sound* Pointeur vers le son chargé
             */
            static Sound *LoadSoundFromMemory(const char *buffer, size_t size, SoundFileType fileType = WAV, SoundType type = SFX);

            /**
             * @brief Joue un son préalablement chargé en mémoire
             * 
             * @param sound L'objet Sound à lire
             * @param loop Joue en boucle si vrai
             * @param pitch Le pitch du son, plus aigu si > 1, et plus grave si < 1
             */
            static void PlaySound(Sound& sound, bool loop = false, float pitch = 1.0f);
    };
}