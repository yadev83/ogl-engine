/**
 * @file aabb.hpp
 * @brief Contient la déclaration de la structure AABB
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine::Physics {
    /**
     * @brief Axis Aligned Bounding Box
     * 
     * Type de "boîte de contour" utilisée par les moteurs physiques pour détecter des collisions
     * L'AABB est alignée sur les axes du "monde" et représente donc un rectangle simple autour d'un objet 
     */
    struct AABB {
        /** @brief La position centrale de l'AABB */
        glm::vec2 center;
        /** @brief La moitié de la taille de l'AABB (sur les deux axes) */
        glm::vec2 halfSize;

        /**
         * @brief Construit un nouvel objet aabb
         * 
         * Calcule la position en fonction de la rotation de sorte que l'AABB englobe toute la zone,
         * même si la rotation implique des coins "vides", l'AABB paraîtra plus grande, mais englobe quand même.
         * 
         * @param pos La position centrale de l'AABB à créer
         * @param size La taille totale (X/Y) de l'AABB
         * @param rot La rotation de l'objet que l'on veut englober (quaternion)
         */
        AABB(glm::vec2 pos, glm::vec2 size, glm::quat rot = glm::quat()) {
            glm::vec2 half = size * 0.5f;

            // Définir les 4 coins dans l'espace local
            glm::vec3 corners[4] = {
                glm::vec3(-half.x, -half.y, 0),
                glm::vec3( half.x, -half.y, 0),
                glm::vec3( half.x,  half.y, 0),
                glm::vec3(-half.x,  half.y, 0)
            };

            // Appliquer la rotation 3D (autour de Z), puis projection 2D
            glm::vec2 min(FLT_MAX), max(-FLT_MAX);
            for (int i = 0; i < 4; ++i) {
                glm::vec3 rotated = rot * corners[i]; // rotation quaternion
                glm::vec2 p = glm::vec2(rotated.x, rotated.y) + pos;

                min = glm::min(min, p);
                max = glm::max(max, p);
            }

            center = (min + max) * 0.5f;
            halfSize = (max - min) * 0.5f;
        }

        /**
         * @brief Renvoie la position la plus faible sur les deux axes (gauche/bas de la box)
         * 
         * @return glm::vec2 
         */
        glm::vec2 Min() const {
            return center - halfSize;
        }

        /**
         * @brief Renvoie la position la plus forte sur les deux axes (droite/haut de la box)
         * 
         * @return glm::vec2 
         */
        glm::vec2 Max() const {
            return center + halfSize;
        }

        /**
         * @brief Renvoie vrai si l'autre AABB passée en paramètre entre en collision avec celle-ci
         * 
         * @param other 
         * @return true 
         * @return false 
         */
        bool Intersects(const AABB& other, float margin = 0.0f) const {
            return (
                (Max().x + margin) > other.Min().x && (Min().x - margin) < other.Max().x &&
                (Max().y + margin) > other.Min().y && (Min().y - margin) < other.Max().y
            );
        }
    };
}