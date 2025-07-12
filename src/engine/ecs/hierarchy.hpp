#pragma once

#include "component.hpp"
#include <set>

namespace Engine::ECS {
    /**
     * @brief Structure de données qui définit un composant "parent" poru stocker un id vers une entité parente dans la hiérarchie si besoin
     * 
     */
    struct Parent : public Component {
        EntityID id;
    };

    /**
     * @brief Equivalent de la structure parent mais pour gérer les enfants dans une entité
     * 
     */
    struct Children : public Component {
        std::set<EntityID> ids;
    };
}