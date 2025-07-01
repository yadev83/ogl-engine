#include "shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../core/logger.hpp"
#include "../utils/filesystem.hpp"

namespace Engine::Graphics {
    Shader::Shader() {}

    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        BuildFromFiles(vertexPath, fragmentPath);
    }

    Shader::Shader(const char *vertexSource, const char *fragmentSource) {
        BuildFromSource(vertexSource, fragmentSource);
    }


    void Shader::BuildFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {        
        std::string vertexShaderSource = Utils::LoadFileToString(vertexPath);
        std::string fragmentShaderSource = Utils::LoadFileToString(fragmentPath);

        BuildFromSource(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
    }

    void Shader::BuildFromSource(const char *vertexSource, const char *fragmentSource) {
        // Compilation of both shader codes
        GLuint vertex, fragment;
        int success; 
        char infoLog[512];

        // Vertex
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            std::cerr << "ERROR : compiling vertex shader: " << std::endl << "  " << infoLog << std::endl;
            mID = 0;
            return;
        }

        // Fragment
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            std::cerr << "ERROR : compiling fragment shader: " << std::endl << "  " << infoLog << std::endl;
            mID = 0;
            return;
        }

        // Shader Program compilation
        mID = glCreateProgram();
        glAttachShader(mID, vertex);
        glAttachShader(mID, fragment);
        glLinkProgram(mID);
        glGetProgramiv(mID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mID, 512, nullptr, infoLog);
            std::cerr << "ERROR : linking shader: " << std::endl << "   " << infoLog << std::endl;
            mID = 0;
            return;
        }

        // Cleanup
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader::~Shader() {
        if(mID) {
            LOG_DEBUG("Destroying shader program");
            glDeleteProgram(mID);
        }
    }
    
    void Shader::Bind() const {
        if (mID == 0) {
            LOG_ERROR("ERROR: attempted to bind an invalid shader program (ID == 0)");
            return;
        }

        glUseProgram(mID);
    }

    void Shader::SetInt(const std::string& name, int val) {
        Bind();
        glUniform1iv(glGetUniformLocation(mID, name.c_str()), 1, &val);
    }

    void Shader::SetBool(const std::string& name, bool val) {
        Bind();
        glUniform1i(glGetUniformLocation(mID, name.c_str()), val);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& mat) {
        Bind();
        glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& vec) {
        Bind();
        glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &vec[0]);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& vec) {
        Bind();
        glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, &vec[0]);
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& vec) {
        Bind();
        glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &vec[0]);
    }

    void Shader::SetFloat(const std::string& name, float val) {
        Bind();
        glUniform1fv(glGetUniformLocation(mID, name.c_str()), 1, &val);
    }
}