#include "font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../utils/filesystem.hpp"

namespace Engine::UI {
    /* FONT */
    Font::Font() {
        LOG_DEBUG("Creating empty font");
    }

    Font::~Font() {
        LOG_DEBUG("Destroying font");
        mCharacters.clear();
        mDataBuffer.clear();
    }
    
    Font::Font(const std::string& fontPath, int fontSize) {
        LOG_DEBUG("Creating Font from file");
        LoadFromFile(fontPath, fontSize);
    }

    Font::Font(const char *buffer, size_t size, int fontSize) {
        LOG_DEBUG("Creating Font from memory");
        LoadFromMemory(buffer, size, fontSize);
    }

    void Font::SetFontSize(float size) {
        mSize = size;

        mCharacters.clear();
        LoadFromMemory(mDataBuffer.data(), mDataBuffer.size());
    }

    float Font::GetFontSize() const {
        return mSize;
    }

    void Font::LoadFromFile(const std::string& fontPath, int fontSize) {
        std::vector<char> fontDataBuffer = Utils::LoadFileToBuffer(fontPath);
        LoadFromMemory(fontDataBuffer.data(), fontDataBuffer.size(), fontSize);
    }

    void Font::LoadFromMemory(const char *buffer, size_t size, int fontSize) {
        LOG_DEBUG("Loading font...");
        mDataBuffer = std::vector<char>(buffer, buffer + size);

        FT_Library ft;
        if(FT_Init_FreeType(&ft)) throw std::runtime_error("Font::Load: ERROR while initializing FreeType");

        FT_Face face;
        if(FT_New_Memory_Face(ft, (unsigned char *)mDataBuffer.data(), static_cast<FT_Long>(mDataBuffer.size()), 0, &face)) 
            throw std::runtime_error("Font::Load: ERROR while loading font");

        FT_Set_Pixel_Sizes(face, 0, fontSize);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                continue;
    
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                         face->glyph->bitmap.width, face->glyph->bitmap.rows,
                         0, GL_RED, GL_UNSIGNED_BYTE,
                         face->glyph->bitmap.buffer);
    
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            FontChar ch = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
    
            mCharacters[c] = ch;
        }
    
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    FontChar Font::GetChar(const char c) {
        return mCharacters[c];
    }
    
    glm::vec2 Font::GetTextSize(const std::string& text, glm::vec2 scale, float maxWidth) {
        glm::vec2 output = {0.0f, 0.0f};
        float maxTop = 0.0f;
        float minBottom = 0.0f;

        for (char c : text) {
            const FontChar& ch = mCharacters.at(c);
            float charWidth = (ch.advance >> 6) * scale.x;
            float top = ch.bearing.y;
            float bottom = ch.bearing.y - ch.size.y;

            if (top > maxTop) maxTop = top;
            if (bottom < minBottom) minBottom = bottom;
            
            output.x += charWidth; // incrémente x pour chaque caractère
        }

        output.x = (maxWidth && output.x > maxWidth) ? maxWidth : output.x;
        output.y = maxTop - minBottom;

        return output;
    }
}