#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Scene {
    void Transform::Translate(const glm::vec3& offset) {
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

    void Transform::SetRotation2D(float angleRadians) {
        rotation = glm::normalize(glm::angleAxis(angleRadians, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f))));
    }
    
    bool Transform::IsRotated() {
        glm::quat id = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return id != rotation;
    }

    glm::vec3 Transform::GetWorldPosition() const {
        if(GetEntity().HasComponent<ECS::Parent>()) {
            EntityID parentID = GetEntity().GetComponent<ECS::Parent>().id;
            if(GetRegistry().IsValidEntity(parentID)) {
                auto parentT = GetRegistry().GetComponent<Transform>(GetEntity().GetComponent<ECS::Parent>().id);
                return parentT.GetWorldPosition() + position;
            }
        }

        return position;
    }

    glm::quat Transform::GetWorldRotation() const {
        if(GetEntity().HasComponent<ECS::Parent>()) {
            EntityID parentID = GetEntity().GetComponent<ECS::Parent>().id;
            if(GetRegistry().IsValidEntity(parentID)) {
                auto parentT = GetRegistry().GetComponent<Transform>(GetEntity().GetComponent<ECS::Parent>().id);
                return parentT.GetWorldRotation() * rotation;
            }
        }

        return rotation;
    }

    glm::vec3 Transform::GetWorldScale() const {
        if(GetEntity().HasComponent<ECS::Parent>()) {
            EntityID parentID = GetEntity().GetComponent<ECS::Parent>().id;
            if(GetRegistry().IsValidEntity(parentID)) {
                auto parentT = GetRegistry().GetComponent<Transform>(GetEntity().GetComponent<ECS::Parent>().id);
                return parentT.GetWorldScale() * scale;
            }
        }

        return scale;
    }

    glm::mat4 Transform::GetLocalMatrix() const {
        return glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 Transform::GetWorldMatrix() const {
        if(GetEntity().HasComponent<ECS::Parent>()) {
            EntityID parentID = GetEntity().GetComponent<ECS::Parent>().id;
            if(GetRegistry().IsValidEntity(parentID)) {
                auto parentT = GetRegistry().GetComponent<Transform>(GetEntity().GetComponent<ECS::Parent>().id);
                return parentT.GetWorldMatrix() * GetLocalMatrix();
            }
        }

        return GetLocalMatrix();
    }
}