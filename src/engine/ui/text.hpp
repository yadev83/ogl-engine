/**
 * @file text.hpp
 * @brief Définit les élements nécessaires pour gérer des textes
 */
#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

#include <engine/ecs/component.hpp>
#include <engine/constants.hpp>
#include <engine/defaults.hpp>

#include "font.hpp"

namespace Engine::UI {
    /**
     * @brief Définit un component "Text" à ajouter à une entité pour créer des interfaces
     * 
     */
    struct Text : public ECS::Component {
        /** @brief le text à afficher */
        std::string text = "";
        /** @brief la taille max autorisée pour ce texte (crop sinon) */
        float maxWidth = 0.0f;
        /** @brief la couleur du texte */
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        
        /** @brief le shader utilisé pour rendre le texte */
        Graphics::Shader *shader = Defaults::textShader;
        /** @brief la police d'écriture utilisée */
        Font *font = Defaults::font;
        /** @brief Une ancre pour positionner le texte */
        Anchor anchor = Anchor::Center;
    };
}