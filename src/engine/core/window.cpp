#include "window.hpp"

#include "../app.hpp"
#include "logger.hpp"

#include <iostream>
#include <stdexcept>

namespace Engine::Core {
    void glErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        LOG_ERROR(std::string(message) + "(" + std::to_string(type) + ")");
        throw std::runtime_error("glError: " + std::string(message) + "(" + std::to_string(type) + ")");
    }

    Window::Window(int width, int height, const char *title) : viewportX(0), viewportY(0), viewportWidth(width), viewportHeight(height) {
        if(!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
        mWindow = glfwCreateWindow(width, height, title, NULL, NULL);
        if(!mWindow) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(mWindow);
    
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(glErrorMessageCallback, 0);
    }
    
    Window::~Window() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }
    
    GLFWwindow *Window::GetRawContext() {
        return this->mWindow;
    }

    void Window::PollEvents() { glfwPollEvents(); }
    void Window::SwapBuffers() { glfwSwapBuffers(mWindow); }
    void Window::BindViewport() { glViewport(viewportX, viewportY, viewportWidth, viewportHeight); }
    bool Window::ShouldClose() const { return glfwWindowShouldClose(mWindow); }
    void Window::Clear(glm::vec4 clearColor) const { 
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::ToggleFullscreen() {
        isFullscreen = !isFullscreen;

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if(isFullscreen) {
            glfwGetWindowPos(mWindow, &mWindowedPosX, &mWindowedPosY);
            glfwGetWindowSize(mWindow, &mWindowedWidth, &mWindowedHeight);

            glfwSetWindowMonitor(mWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(mWindow, nullptr, mWindowedPosX, mWindowedPosY, mWindowedWidth, mWindowedHeight, 0);
        }
    }

    glm::vec2 Window::GetSize() {
        int w, h;
        glfwGetWindowSize(mWindow, &w, &h);
        return glm::vec2(w, h);
    }
}