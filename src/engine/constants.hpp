/**
 * @file constants.hpp
 * @brief Définit des constantes utilisables un peu partout dans le code du moteur
 */
#pragma once

#include <cstdint>
#include <bitset>

namespace Engine {
    /** @brief Définition du maximum d'entités qui peuvent exister dans une scène */
    constexpr std::size_t MAX_ENTITIES = 5000;
    constexpr float COLLISION_EXPIRE_THRESHOLD = 0.1f;
    constexpr float PHYSICS_SLEEP_SPEED_THRESHOLD = 0.5f;
    constexpr float PHYSICS_SLEEP_TIME_THREHSOLD = 1.0f;
    /**
     * @brief Facteur d'echelle pour le système physique.
     * 
     * La simulation physique est basée sur les calculs qu'on pourrait faire dans le monde réel.
     * Mais un mètre dans la vraie vie n'est pas équivalent à un mètre dans l'application.
     * 
     * Ce facteur permet de dire combien d'unités (dans le jeu) sont équivalentes à un mètre (dans la vie)
     * Par défaut, la valeur est de 100u/m, si 1 unité = 1 pixel, ça veut dire que 100 pixels = 1 mètre.
     */
    constexpr float PHYSICS_UNITS_PER_METER = 100.0f;
    /**
     * @brief Facteur de ralentissement dans le vide
     * 
     * Simule une perte de vélocité en l'absence de force, poru que les entités ne soient pas en mouvement perpétuel
     * Par défaut, le ralentissement est léger, mais sur quelques secondes, la vélocité va tendre vers 0.
     * Passer le dampingFactor à 1, désactive toute forme de ralentissement naturel
     */
    constexpr float PHYSICS_DAMPING_FACTOR = 0.99f;
    /** @brief Vitesse d'éxecution de la boucle d'update fixée dans le cycle de vie (en frame per second) */
    constexpr float FIXED_STEP_FRAMERATE = 60.0f;
    /** @brief Le nombre maximum d'itérations autorisés dans les systèmes physiques pour résoudre collisions et contacts */
    constexpr int MAX_PHYSICS_ITERATIONS = 5;
    /**
     * @brief Définit la taille maximale d'une cellule dans un spatial hash (utilisé dans les système physiques)
     * 
     * Plus ce nombre est petit, plus les cellules seront petites (et donc nombreuses)
     * A l'inverse, un trop grand nombre ici crééra d'immenses cellules qui regrouperaient trop d'entités.
     */
    constexpr float SPATIAL_HASH_CELL_SIZE = 250.0f;
}