#include "app.hpp"

#include <iostream>
#include <chrono>
#include <thread>

#include "core/stacktrace.hpp"
#include "core/logger.hpp"
#include "render/debugrenderer.hpp"
#include "input/input.hpp"
#include "utils/resourcemanager.hpp"

namespace Engine {
    void FramebufferSizeCallbackFitToApp(GLFWwindow* window, int width, int height) {
        auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
        if(!app) return;

        if(app->settings.stretchFit) {
            app->GetWindow()->viewportWidth = width;
            app->GetWindow()->viewportHeight = height;
            app->GetRenderTarget()->Resize(width, height);
        } else {
            float targetAspect = (float)app->GetWidth() / (float)app->GetHeight();
            float windowAspect = (float)width / (float)height;

            int viewportWidth, viewportHeight;
            int viewportX = 0, viewportY = 0;

            if(windowAspect > targetAspect) {
                viewportHeight = height;
                viewportWidth = static_cast<int>(height * targetAspect);

                viewportX = (width - viewportWidth) / 2;
            } else {
                viewportWidth = width;
                viewportHeight = static_cast<int>(width / targetAspect);
                viewportY = (height - viewportHeight) / 2;
            }

            // Update viewport position and size
            app->GetWindow()->viewportX = viewportX;
            app->GetWindow()->viewportY = viewportY;
            app->GetWindow()->viewportWidth = viewportWidth;
            app->GetWindow()->viewportHeight = viewportHeight;
            app->GetRenderTarget()->Resize(viewportWidth, viewportHeight);
        }
    }

    App::App(int width, int height, AppSettings settings) : mWidth(width), mHeight(height), settings(settings) {
        Core::EnableStacktrace();

        mWindow = new Core::Window(settings.windowWidth, settings.windowHeight, settings.title.c_str());
        mRenderTarget = new Render::RenderTarget(width, height, settings.resolutionScaling);
        mRenderer = new Render::ScreenRenderer();
        mFrameCounter = 0;

        // Initialisation et traitement OpenGL/GLFW avant d'entrer dans la boucle de l'appli
        glfwSetWindowUserPointer(mWindow->GetRawContext(), this); // Bind un pointeur vers l'App dans le contexte glfw pour accès dans les callbacks
        glfwSetFramebufferSizeCallback(mWindow->GetRawContext(), FramebufferSizeCallbackFitToApp);

        SetSize(mWidth, mHeight, true);
        Input::InputManager::Init(mWindow->GetRawContext());
        Audio::AudioManager::Init();

        // Global registration of a debug renderer system
        RegisterSystem<Render::DebugRenderer>();
        #if defined(NDEBUG)
            GetSystem<Render::DebugRenderer>().Pause();
        #endif
    }

    int App::GetWidth() { return mWidth; }
    int App::GetHeight() { return mHeight; }
    void App::SetSize(int width, int height, bool adjustCamera) {
        mWidth = width;
        mHeight = height;

        if(adjustCamera) {
            auto* mainCamera = GetCurrentCamera();
            if(mainCamera) {
                mainCamera->SetViewSize(GetWidth(), GetHeight());
            }
        }

        if(mWindow && mWindow->GetRawContext()) {
            int contextWidth, contextHeight;
            glfwGetFramebufferSize(mWindow->GetRawContext(), &contextWidth, &contextHeight);
            FramebufferSizeCallbackFitToApp(mWindow->GetRawContext(), contextWidth, contextHeight);
        }
    }
    float App::GetFPS() { return mFrameCounter; }

    Core::Window* App::GetWindow() { return mWindow; }
    Render::RenderTarget* App::GetRenderTarget() { return mRenderTarget; }
    Scene::Scene* App::GetCurrentScene() { return mCurrentScene; }
    Scene::ICamera* App::GetCurrentCamera() { return mCurrentScene ? mCurrentScene->GetCamera() : nullptr; }
    glm::vec2 App::GetMousePosition() {
        glm::vec2 pixelMousePos = Input::InputManager::GetMousePosition(); //pixels, 0,0 en haut à gauche (or, l'app à un 0,0 centré et une rés interne qui peut varier)
        glm::vec2 windowSize = GetWindow()->GetSize();
        glm::vec2 appInternalSize = glm::vec2(GetWidth(), GetHeight());

        // inversion de la hauteur (haut gauch par défaut)
        pixelMousePos.y = windowSize.y - pixelMousePos.y;

        // normalise la position (0 -> 1)
        float normX = pixelMousePos.x / windowSize.x;
        float normY = pixelMousePos.y / windowSize.y;

        // conversion en espace "app" centré en 0,0
        float appX = (normX - 0.5f) * appInternalSize.x;
        float appY = (normY - 0.5f) * appInternalSize.y;

        return glm::vec2(appX, appY);
    }
    glm::vec2 App::GetProjectedMousePosition() {
        glm::vec2 appMousePos = GetMousePosition();
        return appMousePos + GetCurrentCamera()->GetPosition();
    }

    void App::_LoadScene(const std::string& name) {
        if(mCurrentScene) {
            mCurrentScene->OnExit();
            mCurrentScene->GetRegistry()->Clear();

            delete mCurrentScene;
            mCurrentScene = nullptr;
        }

        if(mSceneFactories.find(name) == mSceneFactories.end()) throw std::runtime_error("App::LoadScene: no registered scene matches the given name");
        mCurrentScene = mSceneFactories[name]();
        
        for(auto [_, system]: mSystems) {
            system->mRegistry = mCurrentScene->GetRegistry();
        }
        mCurrentScene->OnEnter();

        mInitialized = false;
    }

    void App::LoadScene(const std::string& name) {
        mSceneToLoad = name;
    }

    void App::Run() {        
        // Time step manager
        float frameStartTime = static_cast<float>(glfwGetTime());
        float deltaTime = 0.0f;
        float lastFrameTime = frameStartTime;

        // Fixed timestep
        float fixedStepAccumulator = 0.0f;
        float fixedStep = 1.0f / FIXED_STEP_FRAMERATE;
        const float maxAccumulator = fixedStep * 5;

        // FPS Counter
        float fpsTimer = 0.0f;
        float fpsCounter = 0.0f;

        // Cycle de vie de l'application (chaque frame) 
        // Init(once) / Update / Render / LateInput
        // Entre chaque, on trouve aussi des fonctions plus "globales" pour maintenir le state opengl pendant l'éxecution
        while(!mWindow->ShouldClose()) {
            // Si une scène à charger est définie, on fait le chargement puis on skip la frame pour éviter les coquilles
            if(mSceneToLoad.has_value()) {
                _LoadScene(mSceneToLoad.value());
                mSceneToLoad.reset();
                continue;
            }

            /* RESETS */
            Input::InputManager::Reset();

            /* TIMING UPDATE */
            frameStartTime = static_cast<float>(glfwGetTime());
            deltaTime = frameStartTime - lastFrameTime;
            lastFrameTime = frameStartTime;

            /* FRAME COUNTER */
            fpsCounter++;
            fpsTimer += deltaTime;
            if(fpsTimer >= 1.0f) {
                mFrameCounter = fpsCounter; // store the latest fps value before resetting

                fpsCounter = 0;
                fpsTimer = 0.0f;
            }

            // Graciously exit if no scene is loaded. Scenes hold registries and are necessary for the lifecycle of the app
            if(!mCurrentScene) {
                LOG_FATAL("Engine has Crashed: Entering App::Run() lifecycle without a valid scene loaded is not allowed");
                return;
            }

            /* INIT (une fois seulement au chargement de la scène. LoadScene remets le booléen à false donc changer de scène appelle OnInit sur tous les systèmes) */
            if(!mInitialized) {
                for(auto [_, system]: mSystems) { system->OnInit(); }
                if(mCurrentScene) { mCurrentScene->OnInit(); }
                mInitialized = true;
            }
            
            // Input first, then lifecycle
            mWindow->PollEvents();
            Input::InputManager::Update(deltaTime);
            // Engine Wide HARDCODED Input bindings
            if(Input::InputManager::GetKey(GLFW_KEY_F9).IsJustPressed() && HasSystem<Render::DebugRenderer>()) {
                auto& debugRenderer = GetSystem<Render::DebugRenderer>();
                if(debugRenderer.IsPaused()) debugRenderer.Resume();
                else debugRenderer.Pause();
            }

            /* FIXED UPDATE */
            // Une update fixée à un timing donné (accumulateur qui trigger ou non l'update à chaque boucle)
            fixedStepAccumulator = (fixedStepAccumulator + deltaTime) < maxAccumulator ? (fixedStepAccumulator + deltaTime) : maxAccumulator;
            while(fixedStepAccumulator >= fixedStep) {
                for(auto [_, system]: mSystems) { system->OnFixedUpdate(fixedStep); }
                if(mCurrentScene) { mCurrentScene->OnFixedUpdate(fixedStep); }
                fixedStepAccumulator -= fixedStep;
            }

            /* UPDATE VARIABLE */
            for(auto [_, system]: mSystems) { if(!system->IsPaused()) system->OnUpdate(deltaTime); }
            if(mCurrentScene) { mCurrentScene->OnUpdate(deltaTime); }
            if(GetCurrentCamera()) { GetCurrentCamera()->OnUpdate(deltaTime); }

            /* RENDER */
            mRenderTarget->Bind();
            mWindow->Clear(settings.clearColor);

            float alpha = fixedStepAccumulator / fixedStep;
            for(auto [_, system]: mSystems) { if(!system->IsPaused()) system->OnRender(alpha); }
            if(mCurrentScene) { mCurrentScene->OnRender(alpha); }

            mRenderTarget->Unbind();
            mWindow->BindViewport();
            mRenderer->RenderToScreen(*mRenderTarget);

            /* UI RENDER (par dessus le rendu écran !) */
            for(auto [_, system]: mSystems) { if(!system->IsPaused()) system->OnUIRender(); }
            if(mCurrentScene) { mCurrentScene->OnUIRender(); }

            // Swap des buffers de mWindow pour acter le rendu
            mWindow->SwapBuffers();

            /* LATE UPDATE */
            for(auto [_, system]: mSystems) { if(!system->IsPaused()) system->OnLateUpdate(deltaTime); }
            if(mCurrentScene) { mCurrentScene->OnLateUpdate(deltaTime); }

            /* SLEEP IF WE ARE AHEAD OF TIME (basé sur le settings.fpsLimit de App, ignoré si fpsLimit <= 0) */
            if(settings.fpsLimit > 0) {
                float targetFrameTime = 1.0f / static_cast<float>(settings.fpsLimit);
                while ((glfwGetTime() - frameStartTime) < targetFrameTime) {}
            }
        }

        mCurrentScene->OnExit();
        Utils::ResourceManager::Clear();
        Audio::AudioManager::Shutdown();
    }

    void App::Quit() {
        glfwSetWindowShouldClose(mWindow->GetRawContext(), true);
    }
}