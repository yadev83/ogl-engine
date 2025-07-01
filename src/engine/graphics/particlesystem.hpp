/**
 * @file particlesystem.hpp
 * @brief Définit le fonctionnement d'un système de particules
 */
#pragma once

#include <glm/glm.hpp>

#include "../ecs/system.hpp"
#include "../ecs/component.hpp"
#include "material.hpp"

namespace Engine::Graphics {
    /**
     * @brief La particule est l'élément atomique du système de particules du moteur.
     * Les émetteurs de particules instancient des entités avec des components "Particle",
     * ensuite, c'est l'émetteur qui gère le cycle de vie de l'entité particule et la paramètre.
     */
    struct Particle : public ECS::Component {
        /** @brief La durée de vie ACTUELLE de la particule */
        float lifetime = 0.0f;
        /** @brief La durée de vie MAXIMALE de la particule (la particule est détruite quand lifetime >= maxLifetime) */
        float maxLifetime = 2.0f;
        /** @brief La vélocité de la particule (utilisée pour définir son comportement) */
        glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

        /** @brief La taille de la particule (peut varier pour créer des effets visuels) */
        float size = 1.0f;
        /** @brief L'opacité de la particule (peut varier pour créer des effets visuels) */
        float initialOpacity = 1.0f;

        /** @brief Active le fade out de la particule au cours de sa durée de vie */
        bool fadeOut = true;
    };


    /**
     * @brief L'emetteur est en quelques sortes le "moteur" qui génère et gère les particules à l'écran
     * Peut être configuré de plein de manière différentes pour permettre des effets visuels customisés.
     */
    struct ParticleEmitter : public ECS::Component {
        /** @brief Le nombre de particules que l'émetteur génère à chaque seconde */
        float emissionRate = 10.0f;
        /** @brief Le temps (en secondes) depuis la dernière fois que l'émetteur à généré des particules */
        float timeSinceLastEmission = 0.0f;
        /** @brief Le nombre maximal de particules que cet émetteur peut gérer simultanément */
        size_t maxParticles = 100;
        /** @brief Si faux, l'émetteur est inactif (et doit être ignoré par les systèmes de particules) */
        bool active = true;
        
        /** @brief La durée de vie minimale d'une particule de cet emetteur */
        float particleMinLifetime = 2.0f;
        /** @brief La durée de vie maximale d'une particule de cet emetteur */
        float particleMaxLifetime = 2.0f;

        /** @brief La vitesse initiale minimale des particules de cet emetteur */
        float particleMinInitialSpeed = 50.0f;
        /** @brief La vitesse initiale maximale des particules de cet emetteur */
        float particleMaxInitialSpeed = 50.0f;

        /** @brief La taille initiale minimale des particules de cet emetteur */
        float particleMinInitialSize = 1.0f;
        /** @brief La taille initiale maximale des particules de cet emetteur */
        float particleMaxInitialSize = 1.0f;

        /** @brief L'opacité minmale de la particule générée */
        float particleMinInitialOpacity = 1.0f;
        /** @brief L'opacité maximale de la particule générée */
        float particleMaxInitialOpacity = 1.0f;

        /** @brief Fait disparaître la particule en fondu via l'opacité au cours du temps */
        float fadeOut = true;
        /** @brief Le material à utiliser pour le rendu des particules de l'émetteur */
        Material sourceMaterial;
    };

    /**
     * @brief La classe ParticleSystem gère le cycle de vie de tous les components de type "ParticleEmitter"
     * 
     */
    class ParticleSystem : public ECS::System {
        public:
            /**
             * @brief Callback d'upadte du système de particules
             * 
             * Pendant l'update, tous les émetteurs sont traités, et s'ils n'ont pas émis de particules
             * depuis assez longtemps, alors le système utilise les paramètres des émetteurs concernés pour en générer de nouvelles.
             * 
             * De plus, toutes les particules encore en vie sont mises à jour par ce callback
             * (incrément du lifetime, et calcul de position/taille/opacité entre chaque frame)
             * 
             * @param deltaTime 
             */
            void OnUpdate(float deltaTime) override;
    };
}