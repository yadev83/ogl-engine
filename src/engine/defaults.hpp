/**
 * @file defaults.hpp
 * @brief Expose des variables par défaut globalement pour les modules qui en ont besoin
 * 
 * Les éléments ci-dessous sont non initialisés pour la plupart au démarrage de l'app.
 * Ils le sont au moment où l'on appelle "App::App()" avec des assets par défaut (data/default.pak).
 * 
 * Ils peuvent tout à fait être écrasés si besoin
 */
#pragma once

#include "ui/font.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

namespace Engine::Defaults {
    /** @brief Une font par défaut, utilisé pour initialiser les éléments ayant besoin d'une police */
    extern UI::Font* font;
    /** @brief Un shader par défaut, utilisé pour initialiser les materials */
    extern Graphics::Shader* shader;
    /** @brief Un shader par défaut pour les textes d'interfaces utilisateur */
    extern Graphics::Shader* textShader;
    /** @brief Une texture par défaut */
    extern Graphics::Texture* texture;
}