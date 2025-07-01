/**
 * @file iniparser.hpp
 * @brief Implémente un lecteur de fichiers ini
 */
#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Engine::Utils {
    /**
     * @brief Définit le type de donnée pour une Section ini
     */
    using IniSection = std::unordered_map<std::string, std::string>;
    /**
     * @brief Définit le type de donnée pour une entrée ini
     */
    using IniData = std::unordered_map<std::string, IniSection>;

    /**
     * @brief La classe parse un fichier INI et stocke la donnée en proposant des helpers pour lire des valeurs
     * 
     */
    class IniParser {
        private:
            IniData mData;
            static std::string Trim(const std::string& s);

        public:
            IniParser() = default;
            /**
             * @brief Charge un fichier ini à l'emplacement défini par le path donné
             * 
             * @param filepath 
             * @return int 
             */
            int Load(const std::string& filepath);

            /**
             * @brief Renvoie une valeur de type string
             * 
             * @param section La section dans laquelle lire
             * @param key La clé dans laquelle se trouve la valeur
             * @param defaultValue Valeur par défaut si la lecture n'est pas possible ou clé absente
             * @return std::string 
             */
            std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
            /**
             * @brief Renvoie une valeur de type int
             * 
             * @param section La section dans laquelle lire
             * @param key La clé dans laquelle se trouve la valeur
             * @param defaultValue Valeur par défaut si la lecture n'est pas possible ou clé absente
             * @return int 
             */
            int GetInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
            /**
             * @brief Renvoie une valeur de type float
             * 
             * @param section La section dans laquelle lire
             * @param key La clé dans laquelle se trouve la valeur
             * @param defaultValue Valeur par défaut si la lecture n'est pas possible ou clé absente
             * @return float 
             */
            float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f) const;
            /**
             * @brief Renvoie une valeur de type bool
             * 
             * @param section La section dans laquelle lire
             * @param key La clé dans laquelle se trouve la valeur
             * @param defaultValue Valeur par défaut si la lecture n'est pas possible ou clé absente
             * @return true 
             * @return false 
             */
            bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false) const;

            /**
             * @brief Permet de checker si une section existe
             * 
             * @param section 
             * @return true 
             * @return false 
             */
            bool HasSection(const std::string& section) const;
            /**
             * @brief Vérifie si une section donnée possède la clé demandée
             * 
             * @param section 
             * @param key 
             * @return true 
             * @return false 
             */
            bool HasKey(const std::string& section, const std::string& key) const;
    };
}