/**
 * @file event.hpp
 * @brief Fichier qui recense les structures et classes de données nécessaires à la gestion d'évènements
 * Permet l'émission et l'écoute d'events customisables à tout endroit du projet.
 */
#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

namespace Engine::Core {
    /**
     * @brief Classe Event dont tous les évènements devront hériter dans le système.
     * Permet simplement de rendre le dispatcher d'évènements "générique" en passant par un type simple comme ça. 
     */
    class Event {
        public:
            virtual ~Event() = default;
    };

    /**
     * @brief EventDispatcher enregistre des listeners, émet des évènements, et appelle des callbacks
     */
    class EventDispatcher {
        private:
            /** @brief spécifie un nom pour les fonctions de callback "génériques" */
            using RawCallback = std::function<void(Event&)>;
            /**
             * @brief Liste de "callbacks" (listeners) pour chaque type d'évènement donné.
             * Utilise le type_index de la classe héritant d'Event qui est utilisé pour créer le listener comme clé
             */
            std::unordered_map<std::type_index, std::vector<RawCallback>> mListeners;
            
        public:
            /** @brief Spécifie un nom pour les fonctions de callbacks typés (par event) */
            template<typename EventType>
            using EventCallback = std::function<void(EventType&)>;

            /**
             * @brief Permet d'enregistrer un callback à appeler lors de l'émission d'un évènement de type EventType
             * 
             * @tparam EventType Le type d'event à écouter
             * @param callback La fonction à appeler si l'évent est émis
             */
            template<typename EventType>
            void Subscribe(EventCallback<EventType> callback) {
                auto& listeners = mListeners[typeid(EventType)];
                listeners.push_back([cb = std::move(callback)](Event& e) {
                    cb(static_cast<EventType&>(e));
                });
            }

            /**
             * @brief Permet d'émettre un évènement de type EventType
             * 
             * @tparam EventType Le type d'évènement à émettre
             * @param event L'objet de type EventType qui est passé en paramètre aux callbacks listeners.
             */
            template<typename EventType>
            void Emit(EventType* event) const {
                auto it = mListeners.find(typeid(EventType));
                if (it != mListeners.end()) {
                    for (const auto& listener : it->second) {
                        listener(*event);
                    }
                }
            }
    };
}