/**
 * @file shader.hpp
 * @brief Classe qui encapsule le chargement let l'usage d'un shader OpenGL
 */
#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Engine::Graphics {
    /**
     * @brief Classe Shader qui porte un identifiant OpenGL pointant vers le shader compilé
     *
     * Contient aussi des méthode utilitaires pour définir des uniforms facilement
     */
    class Shader {
        private:
            /** @brief L'identifiant du shader (permettant d'accéder au programme compilé) */
            GLuint mID;
    
        public:
            GLuint GetID() { return mID; }
            /**
             * @brief Construit un objet Shader et le compile à partir de fichiers vertex/fragment
             * 
             * @param vertexPath Le chemin vers le fichier vertex
             * @param fragmentPath Le chemin vers le fichier fragment
             */
            Shader(const std::string& vertexPath, const std::string& fragmentPath);
            /**
             * @brief Construit un nouveau shader et le compile a partir des sources fournies
             * 
             * @param vertexSource Source vertex
             * @param fragmentPath Source fragment
             */
            Shader(const char *vertexSource, const char *fragmentPath);
            /** @brief Construit un objhet Shader vide */
            Shader();
            /** @brief Détruit proprement un Shader */
            ~Shader();
    
            /**
             * @brief Permet de lier le Shader
             * 
             * Utile pour lier le shader pendant une boucle de rendering avant d'envoyer un mesh au GPU
             */
            void Bind() const;
            /**
             * @brief Compile un programme de Shader à partir de fichiers
             * 
             * @param vertexPath chemin vers le fichier contenant la source .vertex
             * @param fragmentPath chemin vers le fichier contenant la source .fragment
             */
            void BuildFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
            /**
             * @brief Compile un programme de Shader à partir de sources
             * 
             * @param vertexSource code source vertex
             * @param fragmentSource code source fragment
             */
            void BuildFromSource(const char *vertexSource, const char *fragmentSource);
            /**
             * @brief Définit la valeur d'un uniform de type "int"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetInt(const std::string& name, int val);
            /**
             * @brief Définit la valeur d'un uniform de type "float"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetFloat(const std::string& name, float val);
            /**
             * @brief Définit la valeur d'un uniform de type "bool"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetBool(const std::string& name, bool val);
            /**
             * @brief Définit la valeur d'un uniform de type "Mat4"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetMat4(const std::string& name, const glm::mat4& mat);
            /**
             * @brief Définit la valeur d'un uniform de type "Vec4"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetVec4(const std::string& name, const glm::vec4& vec);
            /**
             * @brief Définit la valeur d'un uniform de type "Vec3"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetVec3(const std::string& name, const glm::vec3& vec);
            /**
             * @brief Définit la valeur d'un uniform de type "Vec2"
             * 
             * @param name Le nom du uniform à aller chercher
             * @param val La valeur à envoyer
             */
            void SetVec2(const std::string& name, const glm::vec2& vec);
    };
}