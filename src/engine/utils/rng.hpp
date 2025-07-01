/**
 * @file rng.hpp
 * @brief Utilitaire qui wrap des fonctionnalités de RNG
 */
#pragma once

#include <random>
#include <glm/glm.hpp>
#include <numbers>
#include <algorithm>

namespace Engine::Utils {
    /**
     * @brief Renvoie un nombre décimal aléatoire entre deux valeurs min/max
     * 
     * @param min 
     * @param max 
     * @return float 
     */
    float RandomFloat(float min = 0, float max = RAND_MAX);

    /**
     * @brief Renvoie un angle en radians compris entre les min et max donnés en degrés
     * 
     * @param angleMinDeg 
     * @param angleMaxDeg 
     * @return float 
     */
    float RandomAngle(float angleMinDeg = 0, float angleMaxDeg = 360);
}