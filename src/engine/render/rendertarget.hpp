/**
 * @file rendertarget.hpp
 * @brief Implémentation d'un wrapper autour d'un framebuffer custom pour dessiner une scène
 */
#pragma once

#include "../graphics/shader.hpp"
#include <glm/glm.hpp>

namespace Engine::Render {
    /**
     * @brief La classe RenderTarget encapsule la gestion d'un framebuffer de taille custom pour le rendu de scène en deux temps
     * 
     */
    class RenderTarget {
        private:
            GLuint mFBO, mTextureID;
            bool mGenerated = false;

            /** @brief Résolution interne du Framebuffer, peut différer de la résolution du contexte opengl */
            int mInternalWidth, mInternalHeight;
            /** @brief Facteur de scaling de la résolution. > 1 pour upscale, < 1 pour downscale */
            float mResolutionScalingFactor;
        
        public:
            /**
             * @brief Construit une nouvelle RenderTarget
             * 
             * Génère les ressources OpenGL nécessaires
             * @param width Largeur du viewport original
             * @param height Hauteur du viewport original
             * @param resolutionScaling Facteur de scaling de la résolution du viewport pour obtenir la résolution interne désirée
             */
            RenderTarget(int width, int height, float resolutionScaling = 1.0f);
            /**
             * @brief Détruit la RenderTarget
             * 
             */
            ~RenderTarget();
            
            /**
             * @brief Redimensionne la render target
             * 
             * @param width La nouvelle largeur du viewport original (scaling appliqué en interne)
             * @param height La nouvelle hauteur du viewport original (scaling appliqué en interne)
             */
            void Resize(int width, int height);

            /**
             * @brief Permet de lier la RenderTarget
             * 
             * Après un call Bind(), la RenderTarget réceptionne tous les draw calls OpenGL qui ont lieu
             * Ca permet de dessiner dans le framebuffer custom plutôt que dans le framebuffer de base
             */
            void Bind();
            /**
             * @brief Permet de délier la RenderTarget
             * 
             * Ca aura pour effet de réactiver le Framebuffer par défaut.
             */
            void Unbind();
            /**
             * @brief Renvoie l'identifiant de la texture générée dans le framebuffer custom
             * 
             * @return GLuint 
             */
            GLuint GetTexture();
    };
}