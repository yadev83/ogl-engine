#pragma once

#include <glm/glm.hpp>

namespace Engine::Physics {
    /**
     * @brief Définition d'un manifeste de collision
     * 
     * Contient les infos nécessaires pour "étudier"/"résoudre" une collision
     */
    struct CollisionManifold {
        /** @brief Si vrai, la collision a lieu */
        bool colliding = false;
        /** @brief Vecteur normalisé de direction selon lequel la collision à lieu */
        glm::vec3 normal = {0.0f, 0.0f, 0.0f};
        /** @brief Coefficient de pénétration indiquant la "profondeur" de la collision */
        float penetration = 0.0f;
    };
}