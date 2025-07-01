/**
 * @file defaults.hpp
 * @brief Expose des variables par défaut globalement pour les modules graphiques
 * 
 * Les éléments ci-dessous sont non initialisés pour la plupart, et doivent l'être au démarrage de l'app avant d'être réellement utiles
 */
#pragma once

#include "shader.hpp"
#include "texture.hpp"

namespace Engine::Graphics {
    namespace Defaults {
        /** @brief Un shader par défaut, utilisé pour initialiser les materials */
        extern Shader* shader;
        /** @brief Un shader par défaut pour les textes d'interfaces utilisateur */
        extern Shader* textShader;
        /** @brief Une texture par défaut */
        extern Texture* texture;
    }
}