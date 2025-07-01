/**
 * @file app.hpp
 * @brief Définit la classe App qui est le point d'entrée d'une application dans le moteur.
 * 
 * C'est ici qu'est géré le cycle de vie de l'application, et l'appel aux systèmes ECS et aux scènes
 */
#pragma once

#include <queue>
#include <optional>
#include <array>
#include <typeinfo>

#include "constants.hpp"
#include "utils/statestore.hpp"

#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"

#include "core/window.hpp"
#include "input/input.hpp"
#include "audio/audiomanager.hpp"
#include "scene/scene.hpp"

#include "render/rendertarget.hpp"
#include "render/screenrenderer.hpp"

namespace Engine {
    /**
     * @brief Point d'entrée d'une application. Gère les différents systèmes associés au moteur de jeu
     * 
     * Dans le système ECS, les entités sont mises à jour et "utilisées" par les systèmes en fonction de la signature des composants possédés.
     * Donc l'application ne possède pas directement des entités ou des composants, elle se contente de mettre à jour les systèmes
     * Et ce sont les systèmes qui font le taf sur les entités à chaque loop de cette façon.
     * 
     * App ne gère "que" le cycle de vie des entités/scènes/systèmes dans le moteur de jeu. Le reste dépend des systèmes chargés et des scènes.
     */
    class App {
        private:
            // ECS
            /** @brief Liste des systèmes actuellement chargés dans l'applicaiton */
            std::unordered_map<std::type_index, ECS::System*> mSystems;
            /** @brief Liste des scènes à disposition dans l'application */
            std::unordered_map<std::string, std::function<Scene::Scene*()>> mSceneFactories;
            // Garde en mémoire le nom de la scène à charger (après un appel à LoadScene())
            std::optional<std::string> mSceneToLoad;
            /** @brief Un pointeur vers la scène courante, utilisé pour gérer le cycle de vie en cours */
            Scene::Scene* mCurrentScene = nullptr;
            /** @brief charge une scène (fonction interne appelée si la sceneToLoad est définie) */
            void _LoadScene(const std::string& name);
            /** 
             * @brief Permet de savoir si la phase d'initialisation à eu lieu
             * 
             * Décide si oui ou non, OnInit() doit être appelé sur les classes qui la possèdent (systèmes/scènes)
             */
            bool mInitialized = false;

            // Rendering pipeline
            /** @brief Largeur logique interne de l'application (peut être différente de la largeur du contexte OpenGL) */
            int mWidth;
            /** @brief Hauteur logique interne de l'application (peut être différente de la hauteur du contexte OpenGL) */
            int mHeight;
            /** @brief Contient le nombre d'images comptées depuis la dernière seconde écoulée (FPS) */
            float mFrameCounter;
            /** @brief Pointeur vers Window (wrapper contexte OpenGL) */
            Core::Window* mWindow;
            /** @brief Pointeur vers une renderTarget (utilisée pour un rendu interne à une rés différente avant screenRendering) */
            Render::RenderTarget* mRenderTarget;
            /** @brief Pointeur vers un screenRenderer pour envoyer la renderTarget après génération d'une frame */
            Render::ScreenRenderer* mRenderer;

        public:
            /**
             * @brief Structure de données qui contient des paramètres d'application.
             * 
             * Ces paramètres sont modifiables, et il est conseillé de bien les définir au moment de construire une App 
             */
            struct AppSettings {
                /** @brief Le titre de l'application en cours */
                std::string title; 
                /** @brief largeur de la fenêtre OpenGL générée (peut différer de la taille logique interne de l'app) */
                int windowWidth = 800;
                /** @brief hauteur de la fenêtre OpenGL générée (peut différer de la taille logique interne de l'app) */
                int windowHeight = 600;
                /** @brief Décide si le rendu du renderTarget interne doit s'étirer à la fenêtre ou maintenir l'aspect-ratio */
                bool stretchFit = false;
                /** @brief Multiplie la résolution du renderTarget interne avant de l'envoyer à l'écran (> 1 = upscale, < 1 = downscale, 1 = natif) */
                float resolutionScaling = 1.0f;
                /** @brief La couleur affichée à chaque nettoyage d'écran (avant le dessin de la frame) */
                glm::vec4 clearColor = glm::vec4(0.1f, 0.2f, 0.3f, 1.0f);
                /** 
                 * @brief Limite du nombre d'images par seconde de la boucle principale
                 * Ce paramètre n'influence pas la boucle FixedUpdate qui a son propre framerate interne
                 */
                int fpsLimit = 60;
            } settings;

            /** @brief Store global de l'application */
            Utils::StateStore store;

            /**
             * @brief Créé une nouvelle application
             * 
             * @param width Largeur logique interne de l'application
             * @param height Hauteur logique interne de l'application
             * @param settings Un objet AppSettings pour configurer l'appli
             */
            App(int width = 800, int height = 600, AppSettings settings = {"App"});
            /**
             * @brief Lance la boucle principale de l'application
             * 
             * Run() contient une boucle semi-infinie qui ne s'arrête que lorsque l'on met un terme 
             * à l'éxecution du contexte OpenGL. Chaque itération appelle les fonctions du cycle de vie de l'app. 
             * 
             * Dans l'ordre : OnInit (si !mInitialized) > OnFixedUpdate > OnUpdate > OnRender > OnUIRender > OnLateUpdate
             */
            void Run();
            /** @brief Demande la fermeture du contexte opengl (et par exetnsion met un terme à la boucle Run) */
            void Quit();

            /**
             * @brief Renvoie le wrapper du contexte openGL
             * 
             * @return Core::Window* 
             */
            Core::Window* GetWindow();
            /**
             * @brief Renvoie un pointeur vers la renderTarget utilisée pour le screenRenderer
             * 
             * @return Render::RenderTarget* 
             */
            Render::RenderTarget* GetRenderTarget();
            /**
             * @brief Renvoie un pointeur vers la scène actuellement chargée
             * 
             * @return Scene::Scene* 
             */
            Scene::Scene* GetCurrentScene();
            /**
             * @brief Renvoie un pointeur vers la caméra de la scène actuelle
             * 
             * @return Scene::ICamera* 
             */
            Scene::ICamera* GetCurrentCamera();

            /**
             * @brief Permet d'obtenir la position de la souris dans le cadre de l'application 
             * en prenant en compte les différences de taille entre la fenêtre et l'app
             * 
             * @return glm::vec2 La position {x, y} de la souris dans l'application
             */
            glm::vec2 GetMousePosition();
            /**
             * @brief Permet d'obtenir la position de la osuris projetée dans la scène actuelle
             * 
             * @return glm::vec2 La position {x, y} de la souris dans la scène actuelle
             */
            glm::vec2 GetProjectedMousePosition();

            /**
             * @brief Renvoie la largeur interne de l'application
             * 
             * @return int 
             */
            int GetWidth();
            /**
             * @brief Renvoie la hauteur interne de l'application
             * 
             * @return int 
             */
            int GetHeight();
            /**
             * @brief Définit la taille logique interne de l'application
             * 
             * @param width Nouvelle largeur
             * @param height Nouvelle Hauteur
             * @param adjustCamera Si true => La taille de la caméra courante est mise à jour aussi pour s'adapter à la nouvelle appli.
             */
            void SetSize(int width, int height, bool adjustCamera = true);
            /** @brief Renvoie le nombre d'images comptées depuis la dernière seconde écoulée */
            float GetFPS();

            // Scene Management
            /**
             * @brief Charge la scène demandée
             * 
             * @param name Le nom de la scène à charger
             */
            void LoadScene(const std::string& name);
            /**
             * @brief Enregistre une scène de type T sous un nom donné
             * 
             * @tparam T Le type de la scène enregistré 
             * @param name Le nom donné à la scène dans le registre applicatif
             */
            template<typename T>
            void RegisterScene(const std::string& name) {
                mSceneFactories[name] = [this]() {
                    T* scene = new T();
                    scene->mApp = this;
                    scene->mCamera = new Scene::Camera2D(mWidth, mHeight);

                    return static_cast<Scene::Scene*>(scene);
                };
            }

            // System management
            /**
             * @brief Enregistre un système de type T dans l'application
             * 
             * @tparam T Le type du système à enregistrer
             * @tparam Args Les types d'arguments pour créer le système
             * @param args Les arguments pour créer le système
             * @return T& Une référence vers le système nouvellement enregistré et créé
             */
            template<typename T, typename... Args>
            T& RegisterSystem(Args&&... args) {
                std::type_index ti(typeid(T));
                if(mSystems.find(ti) != mSystems.end()) throw std::runtime_error("App::RegisterSystem: Can not register an already registered system");
                mSystems[ti] = new T(args...);
                mSystems[ti]->mApp = this;
                if(mCurrentScene) mSystems[ti]->mRegistry = mCurrentScene->GetRegistry();

                return static_cast<T&>(*mSystems[ti]);
            }

            /**
             * @brief vérifie si le système de type T est enregistré 
             * 
             * @tparam T 
             * @return bool true si le système existe, false sinon
             */
            template<typename T>
            bool HasSystem() {
                std::type_index ti(typeid(T));
                return mSystems.find(ti) != mSystems.end();
            }

            /**
             * @brief Renvoie le système de type T 
             * 
             * @tparam T 
             * @return T& Une référence vers le système demandé
             */
            template<typename T>
            T& GetSystem() {
                std::type_index ti(typeid(T));
                if(mSystems.find(ti) == mSystems.end()) throw std::runtime_error("App::GetSystem: Can not get an unregistered system");

                return static_cast<T&>(*mSystems[ti]);
            }

            /**
             * @brief Supprime le système de type T de l'application
             * 
             * @tparam T Le type de système à supprimer (désactive le système jusqu'à ce qu'il soit à nouveau enregistré)
             */
            template<typename T>
            void UnregisterSystem() {
                std::type_index ti(typeid(T));
                if(mSystems.find(ti) == mSystems.end()) throw std::runtime_error("App::UnregisterSystem: Can not unregister an already unregistered system");

                delete mSystems[ti];
            }
    };
}