#pragma once

#include <glm/glm.hpp>
#include "../defs.hpp"

namespace Engine::Scene {
    class ICamera {
        private:
            int mViewWidth, mViewHeight;
            glm::vec2 mPosition = {0.0f, 0.0f};
            float mZoom = 1.0f;

        public:
            ICamera(int width, int height);
            ~ICamera() = default;

            void SetPosition(glm::vec2 pos);
            void SetViewSize(int width, int height);
            void SetZoom(float zoom);
            int GetViewWidth() const;
            int GetViewHeight() const;
            glm::vec2 GetPosition() const;
            float GetZoom() const;
            Rectangle GetFrustum() const;

            virtual void OnUpdate(float deltaTime);
            virtual glm::mat4 GetProjectionMatrix() const = 0;
            virtual glm::mat4 GetViewMatrix() const = 0;
    };

    class Camera2D : public ICamera {
        private:
            struct LerpAnimation {
                glm::vec2 start;
                glm::vec2 end;

                bool animating = false;
                float progress = 0.0f;
                float duration = 1.0f;
            } lerpAnim;

        public:
            Camera2D(int width, int height);

            void LerpPosition(glm::vec2 targetPos, float duration = 1.0f);

            void OnUpdate(float deltaTime) override;
            glm::mat4 GetProjectionMatrix() const override;
            glm::mat4 GetViewMatrix() const override;
    };
}