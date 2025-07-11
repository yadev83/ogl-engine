/**
 * @file spatialhash.hpp
 * @brief Définit les structures de données nécessaires à la création d'un spatial hash
 * 
 * Le spatial hash peut être utilisé pour partitionner l'espace dans lequel à lieu des résolutions physiques.
 * L'idée c'est qu'on découpe l'espace global en plusieurs cellules plus petites pour faire un premier "jet" sur la détection de collisions.
 * 
 * Les objets qui ne sont pas dans la même cellule ne sont pas comparés lors de la résolution du système physique, on considère qu'ils sont trop éloignés.
 * Ca évite de faire les checks de collisions plus complexes sur des objets éloignés qui ne sont de toute évidence pas en contact.
 */
#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>

#include "../defs.hpp"

namespace Engine::Physics {
    /**
     * @brief Structure qui définit l'emplacement d'une cellule dans l'espace en 2D
     * 
     * X et Y sont la position de la cellule en "indices" pas en pixels.
     */
    struct Cell { uint32_t x, y; };
    
    /**
     * @brief Surcharge de l'opérateur de test d'égalité pour les cellules
     * 
     * @param a Cellule A
     * @param b Cellule B
     * @return true 
     * @return false 
     */
    bool operator==(const Cell& a, const Cell& b);

    /**
     * @brief Le hash d'une cellule donnée
     * 
     * Il est généré à partir de la position de la cellule (X, Y)
     * On génère un entier de 64 bits dans lequel X et Y sont placés l'un derrière l'autre pour obtenir une signature unique.
     * X et Y faisant chacun 32 bits au maximum
     */
    struct CellHash {
        size_t operator()(Cell c) const;
    };

    /**
     * @brief Défiinition du "type" spatialHash => simplement une unordered_map qui lit des entités à une cellule
     * 
     * On précise aussi que l'unordered_map doit hasher les cellules en utilisant le CellHash que l'on a définit
     */
    using SpatialHash = std::unordered_map<Cell, std::vector<Engine::EntityID>, CellHash>;
}
