/**
 * @file text.hpp
 * @brief Définit les élements nécessaires pour gérer des textes
 */
#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

#include "../ecs/component.hpp"

#include "../defs.hpp"
#include "../graphics/material.hpp"

namespace Engine::UI {
    /**
     * @brief Définit un component "Element" à ajouter à une entité pour créer des interfaces
     * 
     */
    struct Element : public ECS::Component {
        /** @brief la taille de l'élément */
        glm::vec2 size;
        /** @brief le material à appliquer sur l'élément */
        Graphics::Material material;

        /** @brief Une ancre pour positionner l'élément */
        Anchor anchor = Anchor::Center;
    };
}