/**
 * @file defs.hpp
 * @brief Définit des types de données utilisés dans le moteur
 */
#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <bitset>

namespace Engine {
    /** @brief Définition de type pour mieux identifier les EntityID */
    using EntityID = std::uint32_t;
    /** @brief Définition d'une enum "Anchor" qui permet de définir le point d'ancrage d'un item (très utile pour les Interfaces Utilisateur) */
    enum class Anchor {
        BottomLeft,
        Center,
        Bottom,
        Top
    };
    /** @brief Définition d'une structure rectangle qui est défini par la position de deux points : min et max  */
    struct Rectangle {
        glm::vec3 min;
        glm::vec3 max;
    };
}