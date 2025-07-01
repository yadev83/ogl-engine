/**
 * @file debugrenderer.hpp
 * @brief Implémente un renderer capable de dessiner des formes primitives à l'écran, par dessus la scène
 */
#pragma once

#include "../app.hpp"

#include "../graphics/shader.hpp"
#include "../ecs/system.hpp"

namespace Engine::Render {
    /**
     * @brief La classe DebugRenderer embarque un shader non customisable (hardcodé dans le constructeur) et un quad VAO pour faire des rendus
     */
    class DebugRenderer : public ECS::System {
        private:
            /** @brief Le shader "debug" est utilisé pour dessiner les formes à l'écran */
            Graphics::Shader *mDebugShader;
            GLuint mVAO, mVBO, mEBO;

            /**
             * @brief Permet de dessiner une ligne à l'écran entre 2 points a et b
             * 
             * @param a Point de départ de la ligne
             * @param b Point d'arrivée de la ligne
             * @param thickness Epaisseur du trait
             * @param color Couleur du trait
             */
            void DrawLine(glm::vec2 a, glm::vec2 b, float thickness = 1.0f, glm::vec4 color = glm::vec4(1.0f));
            /**
             * @brief Dessine un rectangle (en utilisant 4 lignes)
             * 
             * @param center Le centre du rectangle
             * @param size La taille max du rectangle sur les 2 axes X et Y
             * @param thickness L'épaisseur du trait
             * @param color La couleur
             */
            void DrawRect(glm::vec2 center, glm::vec2 size, float thickness = 1.0f, glm::vec4 color = glm::vec4(1.0f));
            /**
             * @brief Dessine un + à l'endroit designé (en utilisant 2 lignes)
             * 
             * @param center Le centre de la croix
             * @param size La taille de la croix
             * @param thickness L'épaisseur du trait
             * @param color La couleur
             */
            void DrawCross(glm::vec2 center, float size, float thickness = 1.0f, glm::vec4 color = glm::vec4(1.0f));

        public:
            /**
             * @brief Construit un nouvel objet DebugRenderer
             * 
             * Initialise le shader de debug, et le quad vao pour le dessin des primitives
             */
            DebugRenderer();
            /**
             * @brief Destruction de l'objet DebugRenderer
             * 
             * Nettoyage des ressources OpenGL
             */
            ~DebugRenderer();

            /**
             * @brief Fonction du cycle de vie des systèmes ECS
             * 
             * Pour plusieurs "combos" de components, effectue des dessins de debug (rectangles autour des colliders, croix sur les transforms, etc)
             */
            void OnUIRender() override;
    };
}