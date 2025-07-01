#include "rendertarget.hpp"

#include <glad/glad.h>
#include <iostream>

namespace Engine::Render {
    RenderTarget::RenderTarget(int width, int height, float resolutionScaling) : mResolutionScalingFactor(resolutionScaling) {
        Resize(width, height);
    }

    RenderTarget::~RenderTarget() {
        glDeleteFramebuffers(1, &mFBO);
    }

    void RenderTarget::Resize(int width, int height) {
        mInternalWidth = width * mResolutionScalingFactor;
        mInternalHeight = height * mResolutionScalingFactor;

        if(mGenerated) {
            glDeleteFramebuffers(1, &mFBO);
            mGenerated = false;
        }
        
        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_2D, mTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mInternalWidth, mInternalHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR: Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mGenerated = true;
    }

    void RenderTarget::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glViewport(0, 0, mInternalWidth, mInternalHeight);
    }

    void RenderTarget::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint RenderTarget::GetTexture() {
        return mTextureID;
    }
}