/**
 * @file profiler.hpp
 * @brief Le profiler peut être utilisé pour analyser le temps d'éxecution d'un appel de fonction a des fins de débug
 */
#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>

namespace Engine::Core {
    class Profiler {
        private:
            using Clock = std::chrono::high_resolution_clock;
            using TimePoint = Clock::time_point;

            struct ProfileData {
                double totalTime = 0.0;
                int count = 0;
            };

            std::unordered_map<std::string, ProfileData> data;
            TimePoint startTime;
            std::string currentSection;
        
        public:
            /**
             * @brief Démarre une analyse d'éxecution avec le profiler
             * 
             * @param sectionName 
             */
            void Start(const std::string& sectionName);
            
            /**
             * @brief Met fin à la session de profiling en cours
             * 
             */
            void End();

            /**
             * @brief Print dans la console les résultats du profiling pour toutes les sections enregistrées
             * 
             */
            void Report() const;

            /**
             * @brief Réinitialise le profiler
             * 
             */
            void Reset();
    };
}