/**
 * @file sprite.hpp
 * @brief Définit un component "Sprite"
 */
#pragma once

#include <string>
#include <glm/glm.hpp>

#include "../ecs/component.hpp"
#include "material.hpp"

namespace Engine::Graphics {
    /** @brief Structure de donnée contenant de quoi afficher un sprite à l'écran */
    struct Sprite : public ECS::Component {
        /** @brief Le material utilisé pour le rendu du sprite (textures + shader) */
        Graphics::Material material;

        /** @brief Taille du sprite à afficher  */
        glm::vec2 size = {1.0f, 1.0f};
    };
}