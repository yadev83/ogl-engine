/**
 * @file statestore.hpp
 * @brief Implémente un système de state/store typé dynamiquement
 */
#pragma once

#include <unordered_map>
#include <any>
#include <stdexcept>

namespace Engine::Utils {
    class StateStore {
        private:
            std::unordered_map<std::string, std::any> mStore;
            
        public:
            /**
             * @brief Setter typé
             * 
             * @tparam T Type à stocker
             * @param key Clé sous laquelle stocker la donnée
             * @param value La donnée
             */
            template<typename T>
            void SetState(const std::string& key, T value) {
                mStore[key] = value;
            }

            /**
             * @brief Getter avec cast sur le type
             * 
             * @tparam T Type de donné à fetch
             * @param key La clé à chercher dans le store
             * @return T La donnée castée sur le bon type (si possible)
             */
            template<typename T>
            T GetState(const std::string& key) const {
                auto it = mStore.find(key);
                if(it == mStore.end()) throw std::runtime_error("store key not found: " + key);

                try {
                    return std::any_cast<T>(it->second);
                } catch(const std::bad_any_cast& e) {
                    throw std::runtime_error("bad type cast for store key: " + key);
                }
            }

            /**
             * @brief Vérifie si le store contient une clé
             * 
             * @param key 
             * @return true 
             * @return false 
             */
            bool HasKey(const std::string& key) const;
    };  
}