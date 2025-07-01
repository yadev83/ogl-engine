#include "rigidbody.hpp"

namespace Engine::Physics {
    void Rigidbody::AddForce(const glm::vec2& force) {
        AddForce(glm::vec3(force, 0.0f));
    }

    void Rigidbody::AddForce(const glm::vec3& force) {
        if(mass > 0.0f) acceleration += (force / mass);
    }

    void Rigidbody::AddImpulse(const glm::vec2& impulsion) {
        AddImpulse(glm::vec3(impulsion, 0.0f));
    }

    void Rigidbody::AddImpulse(const glm::vec3& impulsion) {
        if(mass > 0.0f) velocity += (impulsion / mass);
    }
}