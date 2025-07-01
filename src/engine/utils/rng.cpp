#include "rng.hpp"

#include <iostream>

namespace Engine::Utils {
    float RandomFloat(float min, float max) {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return min + r * (max - min);
    }

    float RandomAngle(float angleMinDeg, float angleMaxDeg) {
        float min = std::max(0.0f, angleMinDeg);
        float max = std::min(360.0f, angleMaxDeg);

        float angleDeg = RandomFloat(min, max);
        float angleRad = glm::radians(angleDeg);

        return angleRad;
    }
}