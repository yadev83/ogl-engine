#include "logger.hpp"

#include <iostream>
#include <ctime>

namespace Engine::Core {
    Logger::Logger() {
        mLogFile.open("log.txt", std::ios::out | std::ios::trunc);
    }

    Logger::~Logger() {
        if (mLogFile.is_open()) mLogFile.close();
    }

    Logger& Logger::Instance() {
        static Logger instance;
        return instance;
    }

    std::ostream& Logger::Stream() {
        if (mLogFile.is_open()) {
            return mLogFile;
        }
        
        return std::cout;
    }

    void Logger::Log(const std::string& message, LogLevel level, const char *file, int line, const char *func) {
        std::time_t now = std::time(nullptr);
        std::tm * ptm = std::localtime(&now);
        char timestr[32];
        // Format: dd/mm/yyyy hh:mm:ss
        std::strftime(timestr, 32, "%d/%m/%Y %H:%M:%S", ptm);
        
        std::ostream& consoleOut = (level == LogLevel::LOG_ERROR || level == LogLevel::LOG_WARNING) ? std::cerr : std::cout;

        mLogFile << "[" << LogLevelToString(level) << "] " << "("  << timestr << ") " << file << ":" << func << "():" << line << " | " << message << std::endl;
        consoleOut << "[" << LogLevelToString(level) << "] " << "(" << timestr << ") " << file << ":" << func << "():" << line << " | " << message << std::endl;
    }
}