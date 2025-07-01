/**
 * @file input.hpp
 * @brief Définit les inputs clavier/souris dans un contexte OpenGL donné
 */
#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine::Input {
    /**
     * @brief Permet de donner un nom aux différents états dans lesquels peut se trouver une touche
     */
    enum class InputState {
        None,
        JustPressed,
        Pressed,
        JustReleased,
        Released
    };
    
    /**
     * @brief Stocke l'état d'une touche et la durée passée dans l'état
     * 
     * Contient aussi des helpers qui renvoient directement un booléen
     */
    struct KeyState {
        /** @brief Etat de la touche */
        InputState state;
        /** @brief Temps passé dans l'état actuel en secondes */
        float duration = 0.0f;

        bool IsPressed() { return state == InputState::Pressed || state == InputState::JustPressed; }
        bool IsJustPressed() { return state == InputState::JustPressed; }
        bool IsHeld(float durationInSeconds = 0.25f) { return IsPressed() && duration >= durationInSeconds; }
        bool IsReleased() { return state == InputState::Released || state == InputState::JustReleased; }
        bool IsJustReleased() { return state == InputState::JustReleased; }
    };

    /**
     * @brief Classe Statique qui garde un oeil sur l'état de tous les inputs
     * 
     * Utilisable n'importe où pour savoir si une touche est appuyée ou pas, depuis combien de temps, etc...
     * Voir la structure KeyState pour plus de détail et de possibilités.
     */
    class InputManager {
        private:
            /** @brief Pointeur vers le contexte GL actuel */
            static GLFWwindow* mWindow;
            /** @brief Map les identifiants des touches clavier vers des KeyState */
            static std::unordered_map<int, KeyState> mKeys;
            /** @brief Garde en mémoire les KeyState de la frame précédente pour comparaison */
            static std::unordered_map<int, KeyState> mPreviousKeys;
            /** @brief Map les identifiants des boutons souris vers des KeyState */
            static std::unordered_map<int, KeyState> mMouseButtons;
            /** @brief Garde en mémoire les KeyState de la frame précédente pour comparaison */
            static std::unordered_map<int, KeyState> mPreviousMouseButtons;
            /** @brief Contient la valeur du scroll renvoyée par la souris (reset à chaque frame) */
            static float mMouseScrollValue;

            /**
             * @brief Callback GLFW pour récupérer l'offset du scroll de la souris
             * 
             * @param window Le contexte openGL courant
             * @param xoffset L'offset du scroll sur l'axe X (très très rare cas)
             * @param yoffset L'offset du scroll sur l'axe Y (molette souris classique)
             */
            static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        public:
            // Supprime ctor et dtor pour empêcher les erreurs d'inattention à l'utilisation.
            // C'est une classe full statique donc on peut se permettre ça.
            InputManager() = delete; 
            ~InputManager() = delete;

            /**
             * @brief Initialise l'InputManager
             * 
             * Register le/les callbacks nécessaires, et prépare le système d'inputs
             * @param window Le contexte OpenGL courant
             */
            static void Init(GLFWwindow* window);
            /**
             * @brief Mise à jour des états de touches (map KeyState) et des durées d'états
             * 
             * @param deltaTime Temps passé depuis la dernière frame (pour les durrées d'appui)
             */
            static void Update(float deltaTime);
            /**
             * @brief Remise à zéro des états "continus" en fin de frame (scroll souris)
             * 
             */
            static void Reset();

            /**
             * @brief Renvoie le KeyState associé à une touche donnée
             * 
             * @param key La touche pour laquelle on veut le KeyState
             * @return KeyState 
             */
            static KeyState GetKey(int key);
            /**
             * @brief Renvoie la valeur du scroll souris sur l'axe Y
             * 
             * @return float 
             */
            static float GetMouseScroll();

            /**
             * @brief Renvoie la position de la souris (en pixels écran)
             * 
             * @return glm::vec2 
             */
            static glm::vec2 GetMousePosition();
            /**
             * @brief Renvoie le KeyState associé au bouton de souris demandé
             * 
             * @param button Le bouton de souris poru lequel on veut le KeyState
             * @return KeyState 
             */
            static KeyState GetMouseButton(int button);
    };
}