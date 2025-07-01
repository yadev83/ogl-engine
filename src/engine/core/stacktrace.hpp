/**
 * @file stacktrace.hpp
 * @brief Utilise l'API windows mise à disposition pour générer une stacktrace en cas de crash
 */
#pragma once

#include <windows.h>
#include <dbghelp.h>
#include <iostream>
#include <string>

#pragma comment(lib, "dbghelp.lib")

namespace Engine::Core {    
    /**
     * @brief Affiche la stacktrace dans la console
     * 
     * @param context 
     */
    void PrintStackTrace(CONTEXT *context, std::ostream& out = std::cout);

    /**
     * @brief Gère les exceptions en fonction de leur type
     * 
     * @param exceptionInfo 
     * @return LONG 
     */
    LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exceptionInfo);

    /**
     * @brief Renvoie une string associée au code d'exception donné.
     * CF. La doc de Microsoft sur les exceptions C++
     * @param code 
     * @return std::string 
     */
    std::string ExceptionCodeString(DWORD code);
    
    /**
     * @brief Fonction qui active le Handler dans le programme actuel pour activer la stacktrace
     * 
     */
    void EnableStacktrace();
}