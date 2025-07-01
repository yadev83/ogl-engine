/**
 * @file constants.hpp
 * @brief Définit des constantes utilisables un peu partout dans le code du moteur
 */
#pragma once

#include <cstdint>
#include <bitset>

namespace Engine {
    /** @brief Définition de type pour mieux identifier les EntityID */
    using EntityID = std::uint32_t;
    /** @brief Définition du maximum d'entités qui peuvent exister dans une scène */
    constexpr std::size_t MAX_ENTITIES = 5000;
    constexpr float COLLISION_EXPIRE_THRESHOLD = 0.1f;
    /** @brief Vitesse d'éxecution de la boucle d'update fixée dans le cycle de vie (en frame per second) */
    constexpr float FIXED_STEP_FRAMERATE = 60.0f;
    /** @brief Le nombre maximum d'itérations autorisés dans les systèmes physiques pour résoudre collisions et contacts */
    constexpr int MAX_PHYSICS_ITERATIONS = 10;
    /** @brief Définition d'une enum "Anchor" qui permet de définir le point d'ancrage d'un item (très utile pour les Interfaces Utilisateur) */
    enum class Anchor {
        BottomLeft,
        Center
    };
}