/**
 * @file sound.hpp
 * @brief Décrit la structure de donnée pour les sons en mémoire
 * Les sons peuvent être chargés et lus avec l'api OpenAL via le wrapper AudioManager
 */
#pragma once

#include <AL/al.h>
#include "../core/logger.hpp"

namespace Engine::Audio {
    /**
     * @brief SounFileType est une énumération qui liste les types de fichiers audio lisibles par le système
     */
    enum SoundFileType {
        /** @brief WAV est le type de fichier son par défaut du moteur : lossless et facile à lire */
        WAV
    };

    /**
     * @brief SoundType est une énumération qui liste les types de sons dispos dans le moteur
     */
    enum SoundType {
        /** @brief BGM est le type utilisé pour les musiques de fond, qui vont jouer en boucle penadant l'éxecution */
        BGM,   
        /** @brief SFX est le type utilisé pour les "effets" sonores, qui sont joués plusieurs fois */ 
        SFX
    };

    /**
     * @brief Sound est un type de donnée qui contient ce qu'il faut pour jouer un son avec l'api OpenAl
     */
    struct Sound {
        /** @brief Le type de fichier qui est lu dans le buffer  */
        SoundFileType fileType = WAV;
        /** @brief Le type de son stocké */
        SoundType type = SFX;

        /** @brief L'identifiant de source OpenAL pour ce son */
        ALuint source;
        /** @brief L'identifiant du buffer OpenAL qui contient la donnée de ce son */
        ALuint buffer;
        
        /** @brief Le volume auquel doit être joué ce son spécifiquement */
        float volume = 1.0f;

        /**
         * @brief Détruit le son proprement
         * 
         */
        ~Sound() {
            if(source && buffer) {
                LOG_DEBUG("Destroying sound buffer/source");
                alDeleteSources(1, &source);
                alDeleteBuffers(1, &buffer);
            }
        }
    };
}