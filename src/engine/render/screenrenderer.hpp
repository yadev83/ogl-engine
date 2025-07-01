/**
 * @file screenrenderer.hpp
 * @brief 
 */
#pragma once

#include "rendertarget.hpp"
#include "../graphics/shader.hpp"

#include <glad/glad.h>

using namespace Engine::Graphics;

namespace Engine::Render {
    /**
     * @brief La classe ScreenRenderer permet de dessiner une RenderTarget dans le contexte OpenGL courant
     */
    class ScreenRenderer {
        private:
            Shader* mScreenShader;
            GLuint mQuadVAO, mQuadVBO, mQuadEBO;

        public:
            /**
             * @brief Construit un nouveau ScreenRenderer
             * 
             * Génère les ressources openGL nécessaire et un Shader non customisable pour dessiner un framebuffer qui couvre le viewport
             */
            ScreenRenderer();
            /**
             * @brief Détruit le ScreenRenderer et nettoie les ressources OpenGL allouées
             * 
             */
            ~ScreenRenderer();

            /**
             * @brief Dessine une renderTarget à l'écran
             * 
             * @param target La RenderTarget à afficher
             */
            void RenderToScreen(RenderTarget& target);
    };
}