#include "texture.hpp"

#include "../core/logger.hpp"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine::Graphics {
    Texture::Texture() : mID(0) {}
    Texture::Texture(const std::string& path) : mID(0) {
        LoadFromFile(path);
    }
    Texture::Texture(const char *buffer, size_t size) : mID(0) {
        LoadFromMemory(buffer, size);
    }
    Texture::~Texture() {
        if(mID) {
            LOG_DEBUG("Destroying texture");
            glDeleteTextures(1, &mID);
        }
    } 

    void Texture::Build(unsigned char *data, int width, int height, int channels) {
        glGenTextures(1, &mID);
        glBindTexture(GL_TEXTURE_2D, mID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::LoadFromFile(const std::string& path) {
        LOG_DEBUG("Loading Texture from file");
        try {
            // Grab the image buffer from the resource manager
            int width, height, channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
            if(data) {
                Build(data, width, height, channels);
                // Libérer les données CPU
                stbi_image_free(data);
            } else {
                throw std::runtime_error("ERROR during Texture(): stbi_load returned an invalid data buffer"); 
            }
        } catch (std::exception& e) {
            throw std::runtime_error(std::string("ERROR during Texture(): could not construct Texture: ") + e.what());
        }
    }

    void Texture::LoadFromMemory(const char *buffer, size_t size) {
        LOG_DEBUG("Loading Texture from memory");
        try {
            int width, height, channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char *data = stbi_load_from_memory((unsigned char *)(buffer), size, &width, &height, &channels, 0);
            if(data) {
                Build(data, width, height, channels);
                // Libérer les données CPU
                stbi_image_free(data);
            } else {
                throw std::runtime_error("ERROR during Texture::LoadFromMemory(): stbi_load_from_memory returned an invalid data buffer"); 
            }
        } catch (std::exception& e) {
            throw std::runtime_error(std::string("ERROR during Texture::LoadFromMemory(): could not construct Texture: ") + e.what());
        }
    }

    GLuint Texture::Get() const {
        return mID;
    }

    void Texture::Bind(int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, mID);
    }
}