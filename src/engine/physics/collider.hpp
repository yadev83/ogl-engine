/**
 * @file collider.hpp
 * @brief Ce fichier contient les types de données nécessaires à la gestion des collisions dans les systèmes physiques.
 * Un Collider est un component de l'ECS qui permet d'ajouter une "hitbox" autour des entités.
 * On peut ensuite traiter toutes les entités qui ont des colliders et résoudre les collisions entre elles.
 */
#pragma once
#include <functional>

#include "../ecs/component.hpp"
#include "aabb.hpp"

namespace Engine::Physics {
    /**
     * @brief Une entrée qui permet de savoir quelles entités sont en collision avec un Collider
     * On a des maps de ColliderRecords dans les composants de type Collider pour ça,
     * on peut savoir à chaque instant qui est en collision avec le collider, et depuis combien de temps.
     */
    struct ColliderRecord {
        EntityID other;
        float duration = 0.0f;
        bool isTrigger = false;
        bool updatedThisFrame = false;
    };

    /**
     * @brief Le BoxCollider est le collider le plus "simple" qui soit.
     * Il s'agit d'une "box" qui se positionne au centre de l'entité, et s'étend autour d'elle.
     * Pas optimal pour les entités arrondies, mais en 2D, avec des sprites, fait amplement l'affaire.
     */
    struct BoxCollider : public ECS::Component {
        /** @brief Offset par rapport au transform.position de l'entité */
        glm::vec3 offset    = {0.0f, 0.0f, 0.0f};
        /** @brief Taille de la hitbox du collider. Pour des collisions indépendantes du rendu */
        glm::vec3 size      = {1.0f, 1.0f, 1.0f};    
        /** @brief Si true, le collider est un trigger (superposition possible en ne résolvant pas les conditions) */
        bool isTrigger      = false;
        /** @brief Si true, le collider est mis à jour en fonction de l'angle de rotation du transform rattaché */
        bool enableRotation = true;

        /** @brief La box AABB associée à ce collider. Mise à jour par le système physique lors de la gestion des mouvements */
        AABB aabb           = {glm::vec2(0.0f), glm::vec2(0.0f)};
        /** @brief Liste des collisions en cours */
        std::unordered_map<EntityID, ColliderRecord> collisionsList;
        /** @brief Liste des triggers en cours */
        std::unordered_map<EntityID, ColliderRecord> triggersList;
    };
}