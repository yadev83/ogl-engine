/**
 * @file window.hpp
 * @brief Fenêtre/Contexte OpenGL nécessaire pour le rendu 2D/3D et la gestion des inputs pour application de bureau.
 */
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Engine::Core {
    /**
     * @brief Wrapper pour le contexte openGL, ainsi que la statemachine openGL dans une certaine mesure.
     */
    class Window {
        private:
            /** @brief Pointeur vers le contexte raw GLFW */
            GLFWwindow *mWindow;
            /** @brief Garde une trace de l'état actuel du contexte (plein écran si vrai, fenêtre sinon) */
            bool isFullscreen = false;

            /** @brief Largeur du contexte fenêtré (utile pour le swap fullscreen et rétablir le state) */
            int mWindowedWidth;
            /** @brief Hauteur du contexte fenêtré (utile pour le swap fullscreen et rétablir le state) */
            int mWindowedHeight; 
            /** @brief Position X du contexte fenêtré sur le bureau (utile pour le swap fullscreen et rétablir le state) */
            int mWindowedPosX; 
            /** @brief Position Y du contexte fenêtré sur le bureau (utile pour le swap fullscreen et rétablir le state) */
            int mWindowedPosY;
    
        public:
            /** @brief Position X du viewport à l'intérieur du contexte */
            int viewportX;
            /** @brief Position Y du viewport à l'intérieur du contexte */
            int viewportY;
            /** @brief Largeur du viewport */
            int viewportWidth;
            /** @brief Hauteur du viewport */
            int viewportHeight;

            /**
             * @brief Créé un nouveau contexte GLFW 
             * 
             * @param width 
             * @param height 
             * @param title 
             */
            Window(int width, int height, const char *title);
            ~Window();
            /**
             * @brief Renvoie le pointeur vers le contexte GLFW raw
             * 
             * @return GLFWwindow* 
             */
            GLFWwindow *GetRawContext();

            /**
             * @brief Wrapper pour glPollEvents();
             * 
             */
            void PollEvents();

            /**
             * @brief Wrapper pour le buffer swap bindé au contexte GLFW
             * 
             */
            void SwapBuffers();

            /**
             * @brief Applique la taille du viewport sauvegardé dans le contexte actuel
             * 
             */
            void BindViewport();

            /**
             * @brief Wrapper pour la fonction glWindowShouldClose()
             * 
             * @return true 
             * @return false 
             */
            bool ShouldClose() const;

            /**
             * @brief Définit la clear color dans le contexte, et nettoie l'écran
             * 
             * @param clearColor 
             */
            void Clear(glm::vec4 clearColor = glm::vec4(0.0f)) const;

            /**
             * @brief Bascule l'état fullscreen du contexte et change l'état du booléen
             * Au passage en fenêtré les windowedWidth/windowedHeight/windowedX/windowedY sont utilisées pour rétablir la position et taille de la fenêtre
             */
            void ToggleFullscreen();

            /**
             * @brief Renvoie la taille actuelle du contexte
             * 
             * @return glm::vec2 
             */
            glm::vec2 GetSize();
    };
}

