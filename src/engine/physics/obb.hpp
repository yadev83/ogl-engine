/**
 * @file obb.hpp
 * @brief Définit la structure d'une OBB
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine::Physics {
    /**
     * @brief Oriented Bounding Box
     * 
     * Type de "boîte de contour" qui est une version "améliorée" de l'AABB classique.
     * L'OBB est orientée dans la même direction que l'objet qu'elle englobe (rotation)
     * Ca permet des collisions plus précises quand une rotation est impliquée, mais est plus coûteuse à calculer/tester
     */
    struct OBB {
        /** @brief La position centrale de l'OBB */
        glm::vec3 center;
        /** @brief Les 3 axes XYZ orientés de la box */
        glm::vec3 axes[3];
        /** @brief La moitié de la distance sur laquelle chaque axe s'étend */
        glm::vec3 halfExtents;

        /**
         * @brief Construit une nouvelle OBB
         * 
         * @param pos La position centrale de l'objet
         * @param size La taille totale de l'objet
         * @param rot La rotation pour générer les angles orientés
         */
        OBB(glm::vec3 pos, glm::vec3 size, glm::quat rot) {
            center = pos;
            halfExtents = size * 0.5f;

            glm::mat3 rotMat = glm::mat3_cast(rot);

            // Chaque colonne de rotMat est un axe local unitaire
            axes[0] = rotMat[0]; // X local
            axes[1] = rotMat[1]; // Y local
            axes[2] = rotMat[2]; // Z local
        }
    };
}