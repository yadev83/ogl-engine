#include <iostream>
#include <windows.h>

#include <engine/app.hpp>
#include <engine/utils.hpp>
#include <engine/defaults.hpp>
#include <engine/render.hpp>
#include <engine/graphics.hpp>
#include <engine/scene.hpp>
#include <engine/physics.hpp>
using namespace Engine;
using namespace Engine::Scene;
using namespace Engine::Render;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Utils;

#include "scenes/hello_world.hpp"


class Game : public Engine::App {
    public:
        Game(int width = 800, int height = 600, AppSettings settings = {.title="Game"}) : App(width, height, settings) {
            // Registration of engine systems. Comment/remove unused ones if necessary
            RegisterSystem<DebugRenderer>();
            #if defined(NDEBUG)
                GetSystem<DebugRenderer>().Pause();
            #endif
            RegisterSystem<BehaviourSystem>();
            RegisterSystem<PhysicSystem>();
            RegisterSystem<SpriteRenderer>();
            RegisterSystem<SpriteAnimationSystem>();
            RegisterSystem<UIRenderer>();
            RegisterSystem<ParticleSystem>();
            
             // Define defaults variables
            ResourceManager::LoadPak("data/default.pak", "default");
            Defaults::font = ResourceManager::GetFont("default:fonts/default.ttf");
            Defaults::shader = ResourceManager::GetShader("default:shaders/default");
            Defaults::textShader = ResourceManager::GetShader("default:shaders/default_text");
            Defaults::texture = ResourceManager::GetTexture("default:textures/default.png");

            RegisterScene<HelloWorld>("HelloWorld");
            LoadScene("HelloWorld");
        }
};

int _main() {
    IniParser engineConf;
    engineConf.Load("config/engine.ini");

    Engine::App::AppSettings settings = {};
    settings.title = "ogl_engine";
    settings.resolutionScaling = engineConf.GetFloat("DISPLAY", "resolutionScaling", 1.0f);
    settings.stretchFit = (engineConf.GetString("DISPLAY", "resizeMode", "letterbox") == "stretch");
    settings.windowWidth = engineConf.GetInt("WINDOW", "width", 800);
    settings.windowHeight = engineConf.GetInt("WINDOW", "height", 600);
    settings.clearColor = RGBAColor(92, 102, 86);

    Game ogl_engine(1920, 1080, settings);
    ogl_engine.Run();

    return 0;
}


#if defined(NDEBUG)
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
        return _main();
    }
#else
    int main(void) {
        return _main();
    }
#endif