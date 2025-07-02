#include "uirenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../input/input.hpp"
#include "../scene/behaviour.hpp"
#include "../core/logger.hpp"

using namespace Engine::Core;
using namespace Engine::Input;
using namespace Engine::UI;
using namespace Engine::Scene;

namespace Engine::Render {
    UIRenderer::UIRenderer() {
        // Text Buffers
        glGenVertexArrays(1, &mTextVAO);
        glGenBuffers(1, &mTextVBO);
        glBindVertexArray(mTextVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        float quadVertices[] = {
            // pos      // tex
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,

            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.0f, 1.0f
        };

        glGenVertexArrays(1, &mElementVAO);
        glGenBuffers(1, &mElementVBO);

        glBindVertexArray(mElementVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mElementVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    UIRenderer::~UIRenderer() {
        glDeleteBuffers(1, &mTextVBO);
        glDeleteBuffers(1, &mTextVAO);
        glDeleteBuffers(1, &mElementVBO);
        glDeleteBuffers(1, &mElementVAO);
    }

    void UIRenderer::DrawElement(Element element, Transform transform, glm::mat4 projection) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model = glm::scale(model, glm::vec3(element.size, 1.0f) * transform.scale);

        element.material.SetUniform("u_Projection", projection);
        element.material.SetUniform("u_View", glm::mat4(1.0f));
        element.material.SetUniform("u_Model", model);
        element.material.Bind();

        glBindVertexArray(mElementVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void UIRenderer::PrintText(UI::Text text, Scene::Transform transform, glm::mat4 projection) {
        glm::vec2 finalTextPosition = GetTransformedPosition(text, transform);
        
        text.shader->Bind();
        text.shader->SetMat4("u_Projection", projection);
        text.shader->SetMat4("u_View", glm::mat4(1.0f));
        text.shader->SetVec3("u_Color", text.color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(mTextVAO);

        std::string visibleText;

        if(text.maxWidth) {
            // Démarre avec 3 points en longueur (pour les ... à la fin)
            float textWidth = 3 * (text.font->GetChar('.').advance >> 6) * transform.scale.x;
            for(char c : text.text) {
                float advance = (text.font->GetChar(c).advance >> 6) * transform.scale.x;

                if (textWidth + advance > text.maxWidth) {
                    visibleText += "...";
                    break;
                }

                visibleText += c;
                textWidth += advance;
            }
        } else {
            visibleText = text.text;
        }

        float x = finalTextPosition.x;

        for(char c : visibleText) {
            FontChar ch = text.font->GetChar(c);

            float xpos = x + ch.bearing.x * transform.scale.x;
            float ypos = (finalTextPosition.y) - (ch.size.y - ch.bearing.y) * transform.scale.y;

            float w = ch.size.x * transform.scale.x;
            float h = ch.size.y * transform.scale.y;
    
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
    
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
    
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.advance >> 6) * transform.scale.x;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void UIRenderer::OnUpdate(float deltaTime) {
        glm::vec2 mousePos = GetApp().GetMousePosition();
        auto leftMouseBtn = InputManager::GetMouseButton(GLFW_MOUSE_BUTTON_1);

        for(auto entityID : GetRegistry().GetEntityIDsWith<Transform, Element, Action>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& element = GetRegistry().GetComponent<Element>(entityID);
            auto& action = GetRegistry().GetComponent<Action>(entityID);

            bool isInside = IsPointInside(mousePos, transform.position, element.size * glm::vec2(transform.scale));

            if(isInside && !action.hovered) {
                action.hovered = true;
                if(GetRegistry().HasComponent<Behaviour>(entityID)) GetRegistry().GetComponent<Behaviour>(entityID).OnHoverEnter();
            } else if(!isInside && action.hovered) {
                action.hovered = false;
                if(GetRegistry().HasComponent<Behaviour>(entityID)) GetRegistry().GetComponent<Behaviour>(entityID).OnHoverExit();
            }

            if(action.focus != action.prevFocus) {
                if(GetRegistry().HasComponent<Behaviour>(entityID)) {
                    if(action.focus) GetRegistry().GetComponent<Behaviour>(entityID).OnFocusEnter();
                    else GetRegistry().GetComponent<Behaviour>(entityID).OnFocusExit(); 
                }

                action.prevFocus = action.focus;
            }

            if(isInside && leftMouseBtn.IsPressed()) {
                action.pressed = true;
            }

            if(action.pressed && leftMouseBtn.IsReleased()) {
                action.pressed = false;
                if(isInside && GetRegistry().HasComponent<Behaviour>(entityID)) GetRegistry().GetComponent<Behaviour>(entityID).OnSubmit();
            }
        }
    }

    void UIRenderer::OnUIRender() {
        float halfW = (float)GetApp().GetWidth() * 0.5f;
        float halfH = (float)GetApp().GetHeight() * 0.5f;
        glm::mat4 uiProjection = glm::ortho(
            -halfW, halfW,
            -halfH, halfH,
            -1.0f, 1.0f
        );

        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Transform, Element>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& element = GetRegistry().GetComponent<Element>(entityID);

            // Draw centered around center
            if(element.material.shader) {
                DrawElement(element, transform, uiProjection);
            } else {
                LOG_DEBUG("Can not render ui element without a valid shader program");
            }
        }

        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Transform, Text>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& text = GetRegistry().GetComponent<Text>(entityID);

            if(text.shader) {
                // Get la position en fonction de l'ancre de positionnement du texte avant de faire le rendu
                PrintText(text, transform, uiProjection);
            } else {
                LOG_DEBUG("Can not render ui text without a valid shader program");
            }
        }
    }

    glm::vec2 UIRenderer::GetTransformedPosition(Text text, Transform transform) {
        glm::vec2 computedSize = text.font->GetTextSize(text.text, transform.scale, text.maxWidth);

        switch(text.anchor) {
            case Anchor::BottomLeft:
                return glm::vec2(transform.position);

            case Anchor::Top:
                return glm::vec2(transform.position.x - computedSize.x * 0.5f, transform.position.y - computedSize.y);

            case Anchor::Bottom:
                return glm::vec2(transform.position.x - computedSize.x * 0.5f, transform.position.y);

            default:
            case Anchor::Center:
                return (glm::vec2(transform.position) - (computedSize * 0.5f));
        }
    }

    bool UIRenderer::IsPointInside(glm::vec2 point, glm::vec2 targetPosition, glm::vec2 targetSize) {
        glm::vec2 halfSize = targetSize * 0.5f;
        glm::vec2 min = targetPosition - halfSize;
        glm::vec2 max = targetPosition + halfSize;

        return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
    }
}