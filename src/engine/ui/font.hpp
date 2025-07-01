/**
 * @file text.hpp
 * @brief Définit les élements nécessaires pour gérer des textes
 */
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

#include "../ecs/component.hpp"
#include "../constants.hpp"

namespace Engine::UI {
    /**
     * @brief FontChar est une structure qui stocke la data d'un caractère chargé par une font
     * 
     */
    struct FontChar {
        /** @brief La texture OpenGL associée au caractère chargé */
        GLuint textureID = 0;
        /** @brief La taille du caractère en largeur/hauteur */
        glm::ivec2 size;
        /** @brief Le bearing est la distance parcourue par le caractère "dans le vide". */
        glm::ivec2 bearing;
        /** @brief La distance que le caractère parcourt dans le sens de la lecture (utile pour calculer la taille d'une chaine) */
        unsigned int advance;
    };

    /**
     * @brief Charge une police de caractère en utilisant la lib FreeType, et prépare les caractères
     * 
     */
    class Font {
        private:
            std::unordered_map<char, FontChar> mCharacters;
            std::vector<char> mDataBuffer;
            float mSize;
        
        public:
            /**
             * @brief Construit un nouvel objet Font vide
             */
            Font();

            /**
             * @brief Nettoie la font et détruit proprement
             * 
             */
            ~Font();

            /**
             * @brief Construit un nouvel objet Font à partir d'un fichier .ttf
             * 
             * @param fontPath Le chemin vers le fichier font à ouvrir
             * @param fontSize La taille en pixels dans laquelle charger les caractères
             */
            Font(const std::string& fontPath, int fontSize = 48);
            /**
             * @brief Construit un nouvel objet Font à partir d'un buffer mémoire
             * 
             * @param buffer La data de la font
             * @param size La taille du buffer à lire
             * @param fontSize La taille de la police
             */
            Font(const char *buffer, size_t size, int fontSize = 48);

            /**
             * @brief Renvoie la structure FontChar associée à un caractère donné pour cette font
             * 
             * @param c Le caractère dont on veut l'élément graphique
             * @return FontChar 
             */
            FontChar GetChar(const char c);
            /**
             * @brief Change la taille de la police
             * 
             * @param size 
             */
            void SetFontSize(float size);
            /**
             * @brief Permet de lire la taille actuelle de la police
             * 
             * @return float 
             */
            float GetFontSize() const;

            /**
             * @brief Charge une police depuis un fichier sur le disque 
             * 
             * @param filePath Le path vers le fichier à charger
             * @param fontSize La taille de la police
             */
            void LoadFromFile(const std::string& filePath, int fontSize = 48);
            /**
             * @brief Charge une police depuis un buffer en mémoire
             * 
             * @param buffer Le buffer qui contient la data binaire
             * @param size La taille du buffer à lire
             * @param fontSize La taille de la police qu'on veut charger
             */
            void LoadFromMemory(const char *buffer, size_t size, int fontSize = 48);
            /**
             * @brief Renvoie la taille d'une chaine de caractère en fonction des paramètres et de la font
             * 
             * @param text Le texte à mesurer
             * @param scale Modificateur de taille à appliquer au texte
             * @param maxWidth La taille max (pour cropper le texte)
             * @return glm::vec2 La taille précalculée de la chaine
             */
            glm::vec2 GetTextSize(const std::string& text, glm::vec2 scale = glm::vec2(1.0f, 1.0f), float maxWidth = 0.0f);
    };
}