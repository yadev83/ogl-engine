/**
 * @file defaults.hpp
 * @brief Expose des variables par défaut globalement pour les modules graphiques
 * 
 * Les éléments ci-dessous sont non initialisés pour la plupart, et doivent l'être au démarrage de l'app avant d'être réellement utiles
 */
#pragma once

#include "font.hpp"

namespace Engine::UI {
    namespace Defaults {
        /** @brief Une font par défaut, utilisé pour initialiser les éléments ayant besoin d'une police */
        extern Font* font;
    }
}