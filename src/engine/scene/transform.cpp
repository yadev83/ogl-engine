#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Scene {
    void Transform::Translate(const glm::vec3& offset) {
        lastPosition = position;
        position += offset;
    }

    void Transform::Translate(const glm::vec2& offset) {
        Translate(glm::vec3(offset, 0.0f));
    }

    void Transform::Rotate(const glm::vec3& axis, float angleRadians) {
        glm::quat deltaRotation = glm::angleAxis(angleRadians, glm::normalize(axis));
        rotation = deltaRotation * rotation; // Composition : nouvelle rotation d'abord
        rotation = glm::normalize(rotation);
    }

    void Transform::Rotate2D(float angleRadians) {
        Rotate(glm::vec3(0.0f, 0.0f, 1.0f), angleRadians);
    }

    bool Transform::IsRotated() {
        glm::quat id = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return id != rotation;
    }
}