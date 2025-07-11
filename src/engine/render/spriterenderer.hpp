/**
 * @file spriterenderer.hpp
 * @brief Implémente un système qui permet de dessiner des Sprites dans une scène
 */
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../graphics/shader.hpp"
#include "../graphics/material.hpp"
#include "../graphics/sprite.hpp"
#include "../scene/transform.hpp"
#include "../ecs/system.hpp"

namespace Engine::Render {
    /**
     * @brief Système sprite renderer
     * 
     */
    class SpriteRenderer : public ECS::System {
        private:
            GLuint mVAO, mVBO, mEBO;

            /**
             * @brief Dessine un sprite transformé
             * 
             * @param transform La composante transforme associée au sprite
             * @param sprite Le sprite à dessiner
             */
            void DrawSprite(Scene::Transform transform, Graphics::Sprite sprite);

        public:
            /**
             * @brief Construit un nouvel objet SpriteRenderer
             * 
             * Initialise les VAO/VBO/EBO pour dessiner un quad sur lequel envoyer le sprite
             */
            SpriteRenderer();
            /**
             * @brief Détruit l'objet SpriteRenderer
             * 
             * Nettoie les buffers OpenGL
             */
            ~SpriteRenderer();

            /**
             * @brief Fonction du cycle de vie des systèmes
             * 
             * Pour chaque Entité qui possède en simultané au moins un Transform, et un Sprite,
             * effectue le randu du sprite transformé dans la scène.
             * 
             * @param alpha Composante utilisée pour mixer les positions du transform et interpoler en cas de baisse de framerate (unused)
             */
            void OnRender(float alpha = 0.0f) override;
    };
}