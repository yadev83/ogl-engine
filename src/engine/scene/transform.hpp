/**
 * @file transform.hpp
 * @brief 
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <ostream>

#include "../ecs/component.hpp"

namespace Engine::Scene {
    /**
     * @brief Cette structure de donnée permet la gestion d'une entité dans l'espace
     * 
     * ELle donne accès à une position, une rotation et un facteur d'agrandissement permettant le calcul de matrices Model
     */
    struct Transform : public ECS::Component {
        /** @brief La position dans la scène */
        glm::vec3 position              = {0.0f, 0.0f, 0.0f};
        /** @brief La position dans la scène lors de la frame précédente (mis à jour par .Translate() pour les interpolations) */
        glm::vec3 lastPosition          = {0.0f, 0.0f, 0.0f};
        /** @brief La rotation (quaternion) du transform */
        glm::quat rotation              = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        /** @brief Facteur d'argrandissement par lequel tous les components avec une taille sont mis à l'échelle */
        glm::vec3 scale     = {1.0f, 1.0f, 1.0f};

        /**
         * @brief Déplace le component d'un offset donné
         * 
         * @param offset Un vecteur 3D pour la translation
         */
        void Translate(const glm::vec3& offset);
        /**
         * @brief Déplace le component d'un offset donné
         * 
         * @param offset Un vecteur 2D pour la translation
         */
        void Translate(const glm::vec2& offset);
        
        /**
         * @brief Fait tourner l'objet d'un angle donné autour d'un axe
         * 
         * @param axis L'axe autour duquel la rotation à lieu
         * @param angleRadians L'angle de rotation 
         */
        void Rotate(const glm::vec3& axis, float angleRadians);
        /**
         * @brief Encapsule une rotation dans un cadre 2D (autour de l'axe Z)
         * 
         * @param angleRadians L'angle de la rotation
         */
        void Rotate2D(float angleRadians);

        /**
         * @brief Règle l'angle aotour de l'axe Z à l'angle en radians donné
         * 
         * @param angleRadians 
         */
        void SetRotation2D(float angleRadians);

        /**
         * @brief Permet de savoir si l'objet a subi une ou plusieurs rotations
         * 
         * @return true 
         * @return false 
         */
        bool IsRotated();
    };
}