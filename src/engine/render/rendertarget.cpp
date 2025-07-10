#include "rendertarget.hpp"

#include <glad/glad.h>
#include <iostream>

namespace Engine::Render {
    RenderTarget::RenderTarget(int width, int height, float resolutionScaling) : mResolutionScalingFactor(resolutionScaling) {
        Resize(width, height);
    }

    RenderTarget::~RenderTarget() {
        if(mFBO != 0) {
            glDeleteFramebuffers(1, &mFBO);
            mFBO = 0;
        }

        if(mRBO != 0) {
            glDeleteRenderbuffers(1, &mRBO);
            mRBO = 0;
        }
        
        if(mTextureID != 0) {
            glDeleteTextures(1, &mTextureID);
            mTextureID = 0;
        }
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

        glGenRenderbuffers(1, &mRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mInternalWidth, mInternalHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR: Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mGenerated = true;
    }

    void RenderTarget::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glViewport(0, 0, mInternalWidth, mInternalHeight);
        glEnable(GL_DEPTH_TEST);
    }

    void RenderTarget::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint RenderTarget::GetTexture() {
        return mTextureID;
    }
}