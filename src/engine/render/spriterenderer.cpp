#include "spriterenderer.hpp"
#include "../app.hpp"
#include "../core/logger.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace Engine::Graphics;
using namespace Engine::Scene;

namespace Engine::Render {
    SpriteRenderer::SpriteRenderer() {
        // Basic rectangle quad for sprites and basic stuff
        float vertices[] = {
            // Pos        // UV for textures
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLuint mEBO;
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position
        glEnableVertexAttribArray(0); 
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // UV
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    SpriteRenderer::~SpriteRenderer() {
        glDeleteBuffers(1, &mEBO);
        glDeleteBuffers(1, &mVBO);
        glDeleteVertexArrays(1, &mVAO);
    }

    void SpriteRenderer::DrawSprite(Transform transform, Sprite sprite, float alpha) {
        if(sprite.material.shader) {
            glm::vec3 interpolatedPos = glm::length(transform.lastPosition) > 1e-6f ? glm::mix(transform.lastPosition, transform.position, alpha) : transform.position;
            auto mainCamera = GetApp().GetCurrentCamera();

            glm::mat4 model(1.0f);
            model = glm::translate(model, interpolatedPos);
            model *= glm::toMat4(transform.rotation);
            model = glm::scale(model, glm::vec3(sprite.size, 1.0f) * transform.scale);

            sprite.material.Bind();
            sprite.material.shader->SetMat4("u_Projection", mainCamera->GetProjectionMatrix());
            sprite.material.shader->SetMat4("u_View", mainCamera->GetViewMatrix());
            sprite.material.shader->SetMat4("u_Model", model);
            
            glBindVertexArray(mVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        } else {
            LOG_DEBUG("Can not render a sprite without a valid material/shader program");
        }
    }

    void SpriteRenderer::OnRender(float alpha) {
        auto mainCamera = GetApp().GetCurrentCamera();

        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Transform, Sprite>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& sprite = GetRegistry().GetComponent<Sprite>(entityID);

            DrawSprite(transform, sprite, alpha);
        }
    }
}