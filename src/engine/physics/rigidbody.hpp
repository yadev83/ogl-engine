/**
 * @file rigidbody.hpp
 * @brief Définition d'un rigidbody au sens physique du terme
 */
#pragma once

#include <glm/glm.hpp>
#include "../ecs/component.hpp"

namespace Engine::Physics {
    /**
     * @brief La structure rigidbody contient de quoi simuler l'existance d'un corps physique dans le moteur de jeu
     * 
     */
    struct Rigidbody : public ECS::Component {
        /** @brief La vélocité actuelle de l'objet sur 3 axes (dérivation en +position) */
        glm::vec3 velocity      = {0.0f, 0.0f, 0.0f};
        /** @brief L'accélération de l'objet, remise à zéro à chaque frame (dérivation en +vélocité) */
        glm::vec3 acceleration  = {0.0f, 0.0f, 0.0f};
        /** @brief La masse de l'objet, peut servir pour les rapports de force entre deux objets lors de collisions */
        float mass              = 1.0f;
        /** @brief coefficient pour gérer la restitution sur la normale lors des rebonds (0 ne rebondit pas, 1 restitue 100% de la force) */
        float restitution       = 0.4f;
        /** @brief Coefficient qui permet de décider le ralentissement tangentiel en cas de collision / rebond (0, ne ralentit pas : 1, freine instant) */
        float friction          = 0.5f;

        /** @brief Si vrai => L'objet doit être soumis à la gravité des systèmes physiques */
        bool isAffectedByGravity    = true;
        /** @brief Si vrai => Ignore les collisions et forces des systèmes physiques (utile pour gérer à la main tout en gardant la détection de collisiosn) */
        bool isKinematic            = false;
        /** @brief Si vrai => Empêche les mouvements automatiques sur l'axe X */
        bool freezePositionX        = false;
        /** @brief Si vrai => Empêche les mouvements automatiques sur l'axe Y */
        bool freezePositionY        = false;
        /** @brief Si vrai => Empêche les mouvements automatiques sur l'axe Z */
        bool freezePositionZ        = false;
        /** @brief Si vrai => L'objet rebondit en cas de contact physique avec un autre collider */
        bool isBounceable           = false;

        /** @brief Si vrai => Inidque que l'objet est posé à même le sol (ne chute pas) */
        bool onGround               = false;
        /** @brief Si vrai => Indique que l'objet est posé contre un mur */
        bool onWall                 = false;

        /**
         * @brief Ajouter une force en 2D sur le rigidbody
         * 
         * @param force La force physique à appliquer
         */
        void AddForce(const glm::vec2& force);
        /**
         * @brief Ajouter une force en 3D sur le rigidbody
         * 
         * @param force 
         */
        void AddForce(const glm::vec3& force);

        /**
         * @brief Ajouter une impulsion en 2D sur le rigidbody
         * 
         * @param impulsion La impulsion physique à appliquer
         */
        void AddImpulse(const glm::vec2& impulsion);
        /**
         * @brief Ajouter une impulsion en 3D sur le rigidbody
         * 
         * @param impulsion 
         */
        void AddImpulse(const glm::vec3& impulsion);
    };
}