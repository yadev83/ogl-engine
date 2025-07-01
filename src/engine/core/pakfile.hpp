/**
 * @file pakfile.hpp
 * @brief Définit une interface pour lire le contenu des fichiers PAK.
 * Le format PAK étant pensé spécifiquement pour le moteur de jeu, il est utilisé pour bundler les assets par la toolchain de compilation.
 * On peut ensuite charger les .PAK, et récupérer les assets en mémoire.
 */
#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

namespace Engine::Core {
    using Buffer = std::vector<char>;

    /** @brief Structure qui définit le header du type PAK. Utile pour avoir des infos sur ce que l'on va lire */
    struct PakHeader {
        char        format[3];  /** @brief le format du fichier est inscrit dans le header. Doit toujours être "PAK" */
        uint32_t    version;    /** @brief La version du format utilisée. */
        uint32_t    numFiles;   /** @brief Le nombre de fichiers bundlés dans ce fichier PAK */
    };

    /**
     * @brief Classe qui charge un fichier PAK en mémoire.
     * On peut ensuite accéder à ses contenus au format Binaire ou interpréter le texte
     */
    class PakFile {
        private:
            /** @brief Map qui associe le nom initial (avant bundling) d'un fichier, avec son buffer en mémoire */
            std::unordered_map<std::string, Buffer> mFiles;

        public:
            ~PakFile();

            /**
             * @brief Charge un fichier PAK
             * 
             * @param filePath Le chemin vers le fichier à charger
             * @return int 
             */
            int Load(const std::string& filePath);

            /**
             * @brief Récupère la donnée au format buffer binaire
             * 
             * @param name Le nom du fichier à chercher
             * @return Buffer 
             */
            Buffer GetBinaryFile(const std::string& name) const;

            /**
             * @brief Récupère la donnée au format texte
             * Sous le capot, récupère le buffer binaire via GetBinaryFile() et réinterprète en string.
             * 
             * @param name Le nom du fichier à chercher
             * @return std::string 
             */
            std::string GetTextFile(const std::string& name) const;
    };
}