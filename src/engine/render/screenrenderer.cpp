#include "screenrenderer.hpp"

#include <glad/glad.h>

using namespace Engine::Graphics;

namespace Engine::Render {
    ScreenRenderer::ScreenRenderer() {
        // Hard coded screen texture shader.
        // Simple shader that only takes data and throws it 1 to 1 on the gpu shader
        const char *vertexSource = "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "layout (location = 1) in vec2 aTexCoords;\n"
            "\n"
            "out vec2 TexCoords;\n"
            "\n"
            "void main() {\n"
                "TexCoords = aTexCoords;\n"
                "gl_Position = vec4(aPos, 0.0, 1.0);\n"
            "}";

        const char *fragmentSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoords;\n"
            "uniform sampler2D u_ScreenTexture;\n"
            "\n"
            "void main() {\n"
                "FragColor = texture(u_ScreenTexture, TexCoords);\n"
            "}\n";

        mScreenShader = new Shader();
        mScreenShader->BuildFromSource(vertexSource, fragmentSource);

        float quad[] = {
            // positions   // texcoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f,
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &mQuadVAO);
        glBindVertexArray(mQuadVAO);

        glGenBuffers(1, &mQuadVBO);
        glGenBuffers(1, &mQuadEBO);

        glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mQuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    ScreenRenderer::~ScreenRenderer() {
        delete mScreenShader;
        glDeleteVertexArrays(1, &mQuadVAO);
        glDeleteBuffers(1, &mQuadEBO);
        glDeleteBuffers(1, &mQuadVBO);
    }

    void ScreenRenderer::RenderToScreen(RenderTarget& target) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        mScreenShader->Bind();
        glBindVertexArray(mQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, target.GetTexture());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}