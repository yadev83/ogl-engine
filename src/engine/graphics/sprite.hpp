/**
 * @file sprite.hpp
 * @brief Définit un component "Sprite"
 */
#pragma once

#include <string>
#include <glm/glm.hpp>

#include "../ecs/component.hpp"
#include "../ecs/system.hpp"

#include "material.hpp"

namespace Engine::Graphics {
    /** @brief Structure de donnée contenant de quoi afficher un sprite à l'écran */
    struct Sprite : public ECS::Component {
        /** @brief Le material utilisé pour le rendu du sprite (textures + shader) */
        Graphics::Material material;

        /** @brief Taille du sprite à afficher  */
        glm::vec2 size = {1.0f, 1.0f};
    };

    /** @brief Structure de donnée contenant une liste de frames et des paramètres pour de l'animation */
    struct SpriteAnimation {
        /** @brief Liste de frames (Textures) à utiliser pendant l'animation, dans l'ordre d'animation. */
        std::vector<Graphics::Texture *> frames;

        /** @brief Définit la durée (en seconde) d'une frame à l'écran. */
        float frameDuration = 0.1f;
        /** @brief Si true, indique au système d'animation qu'elle doit être répétée une fois arrivée à la dernière frame */
        bool loop = true;
    };

    /**
     * @brief Composant ECS qui stocke plusieurs animations, et permet de jouer l'une ou l'autre avec des appels simples.
     * 
     */
    class SpriteAnimator : public ECS::Component {
        private:
            std::unordered_map<std::string, SpriteAnimation> mAnimations;
            std::string mCurrentAnimationName;
            int mCurrentFrameIndex = 0;
            float mTimer = 0.0f;
            bool mPlaying = false;

        public:
            /**
             * @brief Enregistre une animation dans le SpriteAnimator actuel
             * 
             * @param name Le nom sous lequel l'animation doit être enregistrée
             * @param animation La structure SpriteAnimation à stocker
             */
            void AddAnimation(const std::string& name, const SpriteAnimation& animation);
            /**
             * @brief Renvoie l'objet SpriteAnimation demandé
             * 
             * @param name 
             * @return SpriteAnimation& 
             */
            SpriteAnimation& GetAnimation(const std::string& name);
            /**
             * @brief Permet de jouer l'animation désignée par le nom donnée
             * 
             * @param name Le nom de l'animation à jouer (doit exister dans le SpriteAnimator, sinon rien ne se passe)
             */
            void Play(const std::string& name, bool resetIfSame = false);
            /**
             * @brief Permet d'arrêter l'animation en cours
             * 
             */
            void Stop();

            /**
             * @brief Permet de savoir quelle animation est actuellement en train d'être jouée
             * 
             * @return std::string Renvoie le nom de l'animation en cours 
             */
            std::string GetCurrentAnimationName() const;
            /**
             * @brief Permet de savoir si l'Animator est en train de jouer une animation
             * 
             * @return true L'animation est en cours
             * @return false L'animation est terminée (ou Stop() a été appelé avant)
             */
            bool IsPlaying() const;
            /**
             * @brief Renvoie la texture associée à la frame actuelle dans l'animation en cours
             * 
             * @return Graphics::Texture* Pointeur vers la texture à utiliser dans un sprite pour affichage
             */
            Graphics::Texture* GetCurrentFrame() const;
            /**
             * @brief Mets à jour l'animator en fonction du temps écoulé depuis le dernier appel (passé en paramètre)
             * 
             * Cela implique une mise à jour des timers d'animations, éventuellement un passage de frame.
             * Et potentiellement un arrêt ou une répétition une fois arrivé en bout de course.
             * @param dt 
             */
            void Update(float dt);
    };

    class SpriteAnimationSystem : public ECS::System {
        public:
            /**
             * @brief Callback d'upadte du système d'animation de sprite
             * 
             * Pendant l'update, mets à jour toutes les entités qui ont un sprite et un spriteAnimator
             * L'update commence par mettre à jour les spriteAnimators, et ensuite il met à jour la texture du sprite en fonction de la frame en cours.
             * 
             * @param deltaTime 
             */
            void OnUpdate(float deltaTime) override;
    };
}