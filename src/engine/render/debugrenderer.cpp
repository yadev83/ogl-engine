#include "debugrenderer.hpp"

#include "../app.hpp"

#include "../scene/transform.hpp"
#include "../physics/collider.hpp"
#include "../physics/rigidbody.hpp"
#include "../physics/aabb.hpp"
#include "../ui/text.hpp"
#include "../ui/element.hpp"

#include "../utils/colors.hpp"

using namespace Engine::Graphics;
using namespace Engine::Scene;
using namespace Engine::Physics;
using namespace Engine::UI;

namespace Engine::Render {
    DebugRenderer::DebugRenderer() {
        // Hard coded shape texture shader.
        // Simple shader that only takes data and throws it 1 to 1 on the gpu shader
        const char *vertexSource = "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "uniform mat4 u_Projection;\n"
            "uniform mat4 u_View;\n"
            "uniform mat4 u_Model;\n"
            "\n"
            "void main() {\n"
                "gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 100.0, 1.0);\n"
            "}";

        const char *fragmentSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoords;\n"
            "uniform vec4 u_Color;\n"
            "\n"
            "void main() {\n"
                "FragColor = u_Color;\n"
            "}\n";

        mDebugShader = new Shader();
        mDebugShader->BuildFromSource(vertexSource, fragmentSource);

        float quadVertices[] = {
            // x, y
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f,  0.5f,
            -0.5f,  0.5f
        };

        unsigned int quadIndices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

        // Attrib 0 = position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    DebugRenderer::~DebugRenderer() {
        delete mDebugShader;
        glDeleteBuffers(1, &mEBO);
        glDeleteBuffers(1, &mVBO);
        glDeleteVertexArrays(1, &mVAO);
    }

    void DebugRenderer::DrawLine(glm::vec2 a, glm::vec2 b, float thickness, glm::vec4 color) {
        glm::vec2 center = (a + b) * 0.5f;
        glm::vec2 dir = b - a;
        float length = glm::length(dir);
        float angle = atan2(dir.y, dir.x);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(center, 0.0f));
        model = glm::rotate(model, angle, glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(length, thickness, 1.0f));

        mDebugShader->SetVec4("u_Color", color);
        mDebugShader->SetMat4("u_Model", model);

        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void DebugRenderer::DrawRect(glm::vec2 center, glm::vec2 size, float thickness, glm::vec4 color) {
        glm::vec2 half = size * 0.5f;

        glm::vec2 topLeft     = center + glm::vec2(-half.x,  half.y);
        glm::vec2 topRight    = center + glm::vec2( half.x,  half.y);
        glm::vec2 bottomRight = center + glm::vec2( half.x, -half.y);
        glm::vec2 bottomLeft  = center + glm::vec2(-half.x, -half.y);

        DrawLine(topLeft, topRight, thickness, color);
        DrawLine(topRight, bottomRight, thickness, color);
        DrawLine(bottomRight, bottomLeft, thickness, color);
        DrawLine(bottomLeft, topLeft, thickness, color);
    }

    void DebugRenderer::DrawCross(glm::vec2 center, float size, float thickness, glm::vec4 color) {
        glm::vec2 half = glm::vec2(size * 0.5f);
    
        glm::vec2 horizStart = center - glm::vec2(half.x, 0.0f);
        glm::vec2 horizEnd   = center + glm::vec2(half.x, 0.0f);

        glm::vec2 vertStart = center - glm::vec2(0.0f, half.y);
        glm::vec2 vertEnd   = center + glm::vec2(0.0f, half.y);

        DrawLine(horizStart, horizEnd, thickness, color);
        DrawLine(vertStart, vertEnd, thickness, color);
    }

    void DebugRenderer::OnUIRender() {
        auto mainCamera = GetApp().GetCurrentCamera();
        
        mDebugShader->Bind();
        mDebugShader->SetMat4("u_Projection", mainCamera->GetProjectionMatrix());
        mDebugShader->SetMat4("u_View", mainCamera->GetViewMatrix());
        
        for (auto entityID : GetRegistry().GetEntityIDsWith<Transform>()) {
            const auto& tf = GetRegistry().GetComponent<Transform>(entityID);

            if(!(GetRegistry().HasComponent<Text>(entityID) || GetRegistry().HasComponent<Element>(entityID)))
                DrawCross(tf.position, 10.f, 2.0f, Utils::Colors::BLACK);
        }

        for (auto entityID : GetRegistry().GetEntityIDsWith<Transform, BoxCollider>()) {
            const auto& tf = GetRegistry().GetComponent<Transform>(entityID);
            const auto& col = GetRegistry().GetComponent<BoxCollider>(entityID);

            AABB aabbCollider = AABB(tf.position, col.size * tf.scale, tf.rotation);

            auto color = col.collisionsList.size() ? Utils::Colors::RED : col.triggersList.size() ? Utils::Colors::YELLOW : Utils::Colors::GREEN;
            DrawRect(aabbCollider.center, aabbCollider.halfSize * 2.0f, 2.0f, color);
        }

        for (auto entityID : GetRegistry().GetEntityIDsWith<Transform, Rigidbody>()) {
            const auto& tf = GetRegistry().GetComponent<Transform>(entityID);
            const auto& rb = GetRegistry().GetComponent<Rigidbody>(entityID);
            DrawLine(tf.position, tf.position + (rb.velocity * 50.0f), 1.0f, Utils::Colors::BLUE);
        }
    }
}