/**
 * @file system.hpp
 * @brief Définit la manière dont le Système se comporte dans l'architecture ECS
 * 
 * Les systèmes agissent directement sur des composants en se basant sur l'appartenance commune de certains types
 * de composants à une même entité.
 */
#pragma once

#include "../constants.hpp"
#include <set>

#include "registry.hpp"

namespace Engine {
    class App; // Définition en avance. Pas possible d'inclure app.hpp pour éviter les dépendances circulaires
}

namespace Engine::ECS {
    /**
     * @brief La classe System implémente une logique spécifique à des composants
     * 
     */
    class System {
        // On veut que App puisse accéder aux champs private du System
        friend class Engine::App;

        private:
            /**
             * @brief Pointeur vers le registre actuel
             * 
             * Chaque scène à son propre registre. Donc quand on enregistre un système dans notre appli,
             * on lui précise pour quel registre il doit fonctionner.
             * 
             * La classe App s'occupe de faire pointer vers un nouveau registre en cas de changement de scène pendant l'éxecution.
             */
            Registry* mRegistry;
            /**
             * @brief Pointeur vers l'objet App principal
             * 
             */
            Engine::App* mApp;

            /** @brief Permet de mettre en pause un système complètement. La classe App ignore les systèmes en pause */
            bool mPaused = false;

        public:
            /**
             * @brief Renvoie le registre actif
             * 
             * @return Registry& Référence vers le registre
             */
            Registry& GetRegistry() { return *mRegistry; }
            /**
             * @brief Renvoie l'objet App principal
             * 
             * @return Engine::App& Référence vers l'application
             */
            Engine::App& GetApp() { return *mApp; }

            /** @brief Passe le booléen "mPaused" a false */
            void Resume() { mPaused = false; }
            /** @brief Passe le booléen "mPaused" a true */
            void Pause() { mPaused = true; }
            /**
             * @brief Vérifie si le système est en pause
             * 
             * @return true Si le système est en pause
             * @return false Si le système n'est pas en pause
             */
            bool IsPaused() { return mPaused; }

            virtual ~System() = default;

            /**
             * @brief Fonction d'initialisation dans le cycle de vie de l'app.
             * 
             * L'initialisation à lieu une fois seulement, lors de la première frame qui suit le chargement d'une scène.
             */
            virtual void OnInit() {}
            /**
             * @brief Fonction d'update fixe dans le cycle de vie
             * 
             * L'update fixe est une fonction appelée tant que l'application tourne et que le système est actif.
             * La particularité de la fixed update est qu'elle est timée et est éxecutée un nombre fixe de fois, chaque seconde.
             * Ce peut importe la vitesse d'éxecution réelle de la boucle applicative (dépendant des performances du PC ou de la VSync)
             * @param deltaTime Le temps écoulé depuis le dernier appel à OnFixedUpdate
             */
            virtual void OnFixedUpdate(float deltaTime) {}
            /**
             * @brief Fonction d'update variable dans le cycle de vie
             * 
             * L'update fixe est une fonction appelée tant que l'application tourne et que le système est actif.
             * OnUpdate est appelée à chaque nouvelle frame
             * @param deltaTime Le temps écoulé depuis la frame précédente
             */
            virtual void OnUpdate(float deltaTime) {}
            /**
             * @brief Fonction appelée au rendu à l'écran
             * 
             * @param alpha Coefficient d'interpolation entre deux frames
             */
            virtual void OnRender(float alpha = 0.0f) {}
            /** @brief Fonction appelée après le rendu de l'interface à l'écran */
            virtual void OnUIRender() {}
            /**
             * @brief Fonction d'update tardive appelée à la fin de la boucle
             * 
             * Permet l'éxecution de tâches en retard par rapport a l'update classique et au rendu.
             * 
             * @param deltaTime Le temps écoulé depuis la dernière frame
             */
            virtual void OnLateUpdate(float deltaTime) {}
    };
}