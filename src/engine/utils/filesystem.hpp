/**
 * @file filesystem.hpp
 * @brief Fichier utilitaire qui permet la gestion de fichiers
 */
#pragma once

#include <fstream>
#include <sstream>
#include <vector>

namespace Engine::Utils {
    /**
     * @brief Réplique le fonctionnement des "flux" pour fichiers de C++ sur un buffer mémoire
     * 
     */
    class MemoryStream {
        private:
            const char *mData;
            size_t mSize;
            size_t mCur;

        public:
            /**
             * @brief Ouvre un flux en mémoire sur le buffer donné
             * 
             * @param buffer 
             * @param size 
             */
            MemoryStream(const char *buffer, size_t size);
            /**
             * @brief Lit un nombre donné d'octets dans un pointeur en sortie
             * 
             * @param out Pointeur vers la zone mémoire qui reçoit le contenu lu
             * @param bytesToRead Le nombre d'octets à lire
             * @return int Gestion d'erreurs
             */
            int read(void *out, size_t bytesToRead);
            /**
             * @brief Déplace le curseur à un endroit donné dans le flux
             * 
             * @param position La position où l'on veut se placer
             * @return int 
             */
            int seek(size_t position);
            /**
             * @brief Déplace le curseur d'un nombre d'octets donné depuis sa position actuelle
             * 
             * @param offset Le nombre d'octets dont on se déplace
             * @return int 
             */
            int seekRelative(size_t offset);
            /**
             * @brief Inidique si l'on se trouve à la fin du flux
             * 
             * @return int 
             */
            int eof() const;
            /**
             * @brief Renvoie la position actuelle du curseur dans le flux
             * 
             * @return size_t 
             */
            size_t tell() const;
    };

    /**
     * @brief Permet la lecture d'un fichier à l'emplacement donné sous forme de chaine de caractère
     * 
     * @param path 
     * @return std::string 
     */
    std::string LoadFileToString(const std::string& path);
    /**
     * @brief Permet la lecture d'un fichier à l'emplacement donné sous forme de buffer d'octets
     * 
     * @param path 
     * @return std::vector<char> 
     */
    std::vector<char> LoadFileToBuffer(const std::string& path);
}