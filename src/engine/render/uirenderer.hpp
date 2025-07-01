/**
 * @file uirenderer.hpp
 * @brief Renderer qui affiche les éléments d'UI et gère leurs actions
 */
#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "../ecs/system.hpp"
#include "../scene/transform.hpp"
#include "../ui/text.hpp"
#include "../ui/element.hpp"
#include "../ui/action.hpp"

namespace Engine::Render {
    /**
     * @brief Classe UIRenderer qui s'occupe d'afficher textes et éléments
     * 
     */
    class UIRenderer : public ECS::System {
        private:
            GLuint mTextVAO, mTextVBO;
            GLuint mElementVAO, mElementVBO;

            glm::vec2 GetTransformedPosition(UI::Text text, Scene::Transform transform);
            bool IsPointInside(glm::vec2 point, glm::vec2 targetPosition, glm::vec2 targetSize);

            void PrintText(UI::Text text, Scene::Transform transform, glm::mat4 projection);
            void DrawElement(UI::Element element, Scene::Transform transform, glm::mat4 projection);

        public:
            /**
             * @brief Construit un objet UIRenderer
             * 
             * Met en place les ressources OpenGL pour les textes et les éléments 
             */
            UIRenderer();
            /**
             * @brief Détruit l'objet UIRenderer
             * 
             * Libère les ressources OpenGL
             */
            ~UIRenderer();

            /**
             * @brief Lifecycle de l'app
             * 
             * Gère les inputs souris et les hoverIn/hoverOut
             * 
             * @param dt Le temps écoulé depuis la dernière fraime
             */
            void OnUpdate(float dt) override;
            /**
             * @brief Lifecycle de l'app
             * 
             * Affiche les élements de l'interface par dessus la scène 
             */
            void OnUIRender() override;
    };
}