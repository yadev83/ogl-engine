/**
 * @file logger.hpp
 * @brief Système de logs console/fichier
 */
#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

#ifdef SOURCE_ROOT
    #define RELATIVE_FILE (__FILE__ + sizeof(SOURCE_ROOT) - 1)
#else
    #define RELATIVE_FILE __FILE__
#endif

/** @brief Définition d'une macro pour envoyer un Log de type "INFO" */
#define LOG_INFO(msg) Engine::Core::Logger::Instance().Log(msg, Engine::Core::LogLevel::LOG_INFO, RELATIVE_FILE, __LINE__, __func__)
/** @brief Définition d'une macro pour envoyer un Log de type "WARNING" */
#define LOG_WARNING(msg) Engine::Core::Logger::Instance().Log(msg, Engine::Core::LogLevel::LOG_WARNING, RELATIVE_FILE, __LINE__, __func__)
/** @brief Définition d'une macro pour envoyer un Log de type "ERROR" */
#define LOG_ERROR(msg) Engine::Core::Logger::Instance().Log(msg, Engine::Core::LogLevel::LOG_ERROR, RELATIVE_FILE, __LINE__, __func__)
/** @brief Définition d'une macro pour envoyer un log de type "FATAL" */
#define LOG_FATAL(msg) Engine::Core::Logger::Instance().Log(msg, Engine::Core::LogLevel::LOG_FATAL, RELATIVE_FILE, __LINE__, __func__)
/** @brief Définition d'une macro pour envoyer un Log de type "DEBUG" (NOP en build de Release) */
#if defined(NDEBUG)
    #define LOG_DEBUG(msg) do {} while (0)
#else
    #define LOG_DEBUG(msg) Engine::Core::Logger::Instance().Log(msg, Engine::Core::LogLevel::LOG_DEBUG, RELATIVE_FILE, __LINE__, __func__)
#endif

namespace Engine::Core {
    /** @brief Enumération qui définit les types de logs à disposition */
    enum class LogLevel {
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
        LOG_FATAL
    };

    /**
     * @brief Classe Singleton "Logger".
     * Ne devrais jamais être appelée directement, puisqu'il existe des macros toutes prêtes.
     */
    class Logger {
        private:
            Logger();
            ~Logger();

            /** @brief Un stream vers le fichier de log ouvert pour l'écriture sans console */
            std::ofstream mLogFile;

        public:
            /**
             * @brief Renvoie l'instance du singleton Logger
             * 
             * @return Logger& 
             */
            static Logger& Instance();
            /**
             * @brief Envoie un message de log dans la console, et dans le fichier ouvert.
             * 
             * @param message Le message à envoyer
             * @param level Le niveau de gravité du message (INFO<WARNING<ERROR). Debug n'apparaît qu'en build de Debug
             * @param file Une string contenant le nom du fichier source qui appelle émet le log
             * @param line La ligne qui a émis le log
             * @param func Le nom de la fonction qui émet le log
             */
            void Log(const std::string& message, LogLevel level = LogLevel::LOG_INFO, const char *file = "", int line = 0, const char *func = "");

            /**
             * @brief Renvoie un stream vers le fichier de log actuellement ouvert
             * 
             * Renvoie : std::cout si le fichier n'est pas ouvert
             * 
             * @return std::ostream& 
             */
            std::ostream& Stream();
    };

    /**
     * @brief Fonction utilitaire qui revnoie une string associé à un niveau de log
     * Utile pour afficher la sévérité du log dans le message formatté
     * 
     * @param level 
     * @return const char* 
     */
    inline const char* LogLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::LOG_DEBUG: return "DEBUG";
            case LogLevel::LOG_INFO: return "INFO";
            case LogLevel::LOG_WARNING: return "WARNING";
            case LogLevel::LOG_ERROR: return "ERROR";
            case LogLevel::LOG_FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
}