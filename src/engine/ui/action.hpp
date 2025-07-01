/**
 * @file action.hpp
 * @brief Définit une action d'interface utilisateur
 */
#pragma once

#include "../ecs/component.hpp"

namespace Engine::UI {
    /** @brief Structure de donnée qui indique si l'élément cible est survolé par la souris */
    struct Action : public ECS::Component {
        /** @brief si vrai => La souris est par dessus l'élément dans l'interface */
        bool hovered    = false;
        /** @brief si vrai => La souris est par dessus l'élément ET le bouton principal est appuyé */
        bool pressed    = false;
        /** @brief si vrai => L'élément est préselectionné. A coupler avec une navigation manette/clavier des UI */
        bool focus      = false;
        /** @brief gère l'entrée/sortie du mode focus via des comparaisons avec la valeur focus */
        bool prevFocus  = false;
    };
}