#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Scene {
    /* Camera Interface */
    ICamera::ICamera(int width, int height) : mViewWidth(width), mViewHeight(height) {}

    void ICamera::SetViewSize(int width, int height) {
        mViewWidth = width;
        mViewHeight = height;
    }

    void ICamera::SetZoom(float zoom) {
        mZoom = zoom;
    }

    glm::vec2 ICamera::GetPosition() const { return mPosition; }

    void ICamera::SetPosition(glm::vec2 pos) {
        mPosition = pos;
    }

    void ICamera::OnUpdate(float deltaTime) {}
    int ICamera::GetViewHeight() const { return mViewHeight; }
    int ICamera::GetViewWidth() const { return mViewWidth; }
    float ICamera::GetZoom() const { return mZoom; }

    /* CAMERA 2D Implementation */
    Camera2D::Camera2D(int width, int height) : ICamera(width, height) {}

    void Camera2D::LerpPosition(glm::vec2 targetPos, float duration) {
        lerpAnim.animating = true;
        lerpAnim.start = GetPosition();
        lerpAnim.end = targetPos;
        lerpAnim.progress = 0.0f;
        lerpAnim.duration = duration;
    }

    void Camera2D::OnUpdate(float deltaTime) {
        // Handle lerpAnimation
        if(lerpAnim.animating) {
            lerpAnim.progress += deltaTime / lerpAnim.duration;
            if(lerpAnim.progress >= 1.0f) {
                lerpAnim.progress = 1.0f;
                lerpAnim.animating = false;
            }
            SetPosition(glm::mix(lerpAnim.start, lerpAnim.end, lerpAnim.progress));
        }
    }

    glm::mat4 Camera2D::GetProjectionMatrix() const {
        float halfW = static_cast<float>(GetViewWidth()) * 0.5f / GetZoom();
        float halfH = static_cast<float>(GetViewHeight()) * 0.5f / GetZoom();

        return glm::ortho(-halfW, halfW, -halfH, halfH, -1000.0f, 1000.0f);
    }

    glm::mat4 Camera2D::GetViewMatrix() const {
        glm::mat4 view = glm::mat4(1.0f);

        // offset center
        view = glm::translate(view, glm::vec3(-GetPosition(), 0.0f));

        // Isometric rotation test
        // view = glm::rotate(view, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // view = glm::rotate(view, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        return view;
    }
}