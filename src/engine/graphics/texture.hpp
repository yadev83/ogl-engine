/**
 * @file texture.hpp
 * @brief Définit la classe Texture utilisée dans pas mal de renderers graphiques   
 * Son principal intérêt est de permettre le stockage d'un ID pour une texture OpenGL ainsi que le bind/get de cet ID
 */
#pragma once

#include <iostream>
#include <glad/glad.h>

#include <glm/glm.hpp>

namespace Engine::Graphics {
    /**
     * @brief Transporte un ID de texture OpenGL
     * 
     */
    class Texture {
        private:
            /** @brief L'identifiant renvoyé par OpenGL quand on build la texture */
            GLuint mID;
            /** @brief Génère un identifiant unique et charge la texture à partir du buffer donné */
            void Build(unsigned char *data, int width, int height, int channels);

        public:
            /** @brief Le mode de wrapping de la texture openGL (cf. doc opengl pour les options possibles) */
            GLenum glWrappingMode = GL_REPEAT;

            /**
             * @brief Construit un nouvel objet Texture et charge la texture demandée
             * 
             * @param path Le chemin vers la texture à charger
             */
            Texture(const std::string& path);
            /**
             * @brief Construit un nouvel objet Texture et charge la texture depuis un buffer mémoire
             * 
             * @param buffer La donnée binaire à charger
             * @param size La taille du buffer
             */
            Texture(const char *buffer, size_t size);
            /**
             * @brief Construit un nouvel object Texture vide
             */
            Texture();
            /**
             * @brief Détruit proprement la texture
             * 
             */
            ~Texture();

            /**
             * @brief Renvoie la taille de la texture
             * 
             * @return glm::ivec2 
             */
            glm::ivec2 GetSize();

            /**
             * @brief Charge un fichier texture et construit la texture OpenGL
             * 
             * @param path Chemin vers l'image à charger
             */
            void LoadFromFile(const std::string& path);
            /**
             * @brief Charge une texture depuis le buffer donné
             * 
             * @param buffer Donnée de la texture à charger
             * @param size Taille du buffer
             */
            void LoadFromMemory(const char *buffer, size_t size);

            /**
             * @brief Renvoie l'identifiant de la texture
             * 
             * @return GLuint 
             */
            GLuint Get() const;
            /**
             * @brief Lie la texture à l'unité demandée
             * 
             * @param unit Unité de bind pour la texture (GL_TEXTURE_0->16)
             */
            void Bind(int unit = 0) const;
    };
}