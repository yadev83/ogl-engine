#include "input.hpp"

namespace Engine::Input {
    GLFWwindow *InputManager::mWindow;
    
    std::unordered_map<int, KeyState> InputManager::mKeys;
    std::unordered_map<int, KeyState> InputManager::mPreviousKeys;
    std::unordered_map<int, KeyState> InputManager::mMouseButtons;
    std::unordered_map<int, KeyState> InputManager::mPreviousMouseButtons;
    float InputManager::mMouseScrollValue;
    
    void InputManager::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        mMouseScrollValue = static_cast<float>(yoffset);
    }


    void InputManager::Init(GLFWwindow* window) {
        mWindow = window;
        glfwSetScrollCallback(mWindow, MouseScrollCallback);
    }

    void InputManager::Update(float deltaTime) {
        mPreviousKeys = mKeys;
        //mMouseScrollValue = 0.0f;

        for(int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
            bool isPressed = glfwGetKey(mWindow, key) == GLFW_PRESS; // regarde l'état actuel de la touche via glfw
            bool wasPressed = mPreviousKeys[key].IsPressed(); // regarde l'état précédent
            
            // En comparant les deux, on peut mettre à jour le state de manière assez précise
            if(isPressed) {
                mKeys[key].state = wasPressed ? InputState::Pressed : InputState::JustPressed;
                mKeys[key].duration = wasPressed ? (mKeys[key].duration + deltaTime) : 0.0f;
            } else {
                mKeys[key].state = wasPressed ? InputState::JustReleased : InputState::Released;
                mKeys[key].duration = wasPressed ? 0.0f : (mKeys[key].duration + deltaTime);
            }
        }

        mPreviousMouseButtons = mMouseButtons;
        for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
            bool isPressed = glfwGetMouseButton(mWindow, button) == GLFW_PRESS; // regarde l'état actuel de la touche via glfw
            bool wasPressed = mPreviousMouseButtons[button].IsPressed(); // regarde l'état précédent
            
            // En comparant les deux, on peut mettre à jour le state de manière assez précise
            if(isPressed) {
                mMouseButtons[button].state = wasPressed ? InputState::Pressed : InputState::JustPressed;
                mMouseButtons[button].duration = wasPressed ? (mMouseButtons[button].duration + deltaTime) : 0.0f;
            } else {
                mMouseButtons[button].state = wasPressed ? InputState::JustReleased : InputState::Released;
                mMouseButtons[button].duration = wasPressed ? 0.0f : (mMouseButtons[button].duration + deltaTime);
            }
        }
    }

    void InputManager::Reset() {
        mMouseScrollValue = 0.0f;
    }

    KeyState InputManager::GetKey(int key) {
        return mKeys[key];
    }

    float InputManager::GetMouseScroll() {
        return mMouseScrollValue;
    }
    
    glm::vec2 InputManager::GetMousePosition() {
        double x, y;
        glfwGetCursorPos(mWindow, &x, &y);

        return glm::vec2((float)x, (float)y);
    }

    KeyState InputManager::GetMouseButton(int button) {
        return mMouseButtons[button];
    }
}