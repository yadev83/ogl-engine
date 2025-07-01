/**
 * @file colors.hpp
 * @brief Utilitaires pour la gestion de couleurs
 */
#pragma once

#include <glm/glm.hpp>

namespace Engine::Utils {
    /**
     * @brief Fonction qui renvoie un vec4 normalisé pour les couleurs à partir de valeurs RGBA
     * 
     * @param r Valeur entre 0 et 255
     * @param g Valeur entre 0 et 255
     * @param b Valeur entre 0 et 255
     * @param alpha Valeur entre 0 et 255
     * @return glm::vec4 
     */
    glm::vec4 RGBAColor(int r, int g, int b, int alpha = 255);

    /**
     * @brief Contient des couleurs "constantes" classiques pour gagner du temps si besoin
     * 
     */
    namespace Colors {
        constexpr glm::vec4 RED = {1.0f, 0.0f, 0.0f, 1.0f};
        constexpr glm::vec4 YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
        constexpr glm::vec4 GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
        constexpr glm::vec4 CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
        constexpr glm::vec4 BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
        constexpr glm::vec4 MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
        constexpr glm::vec4 WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
        constexpr glm::vec4 BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
    }
}