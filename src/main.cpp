#include <iostream>
#include <windows.h>
#include <engine/app.hpp>
#include <engine/utils/iniparser.hpp>
#include <engine/utils/colors.hpp>

#include <engine/render/spriterenderer.hpp>
#include <engine/render/uirenderer.hpp>
#include <engine/render/debugrenderer.hpp>
#include <engine/graphics/particlesystem.hpp>
#include <engine/scene/behavioursystem.hpp>
#include <engine/physics/physicsystem.hpp>

using namespace Engine::Scene;
using namespace Engine::Render;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Utils;

class Game : public Engine::App {
    public:
        Game(int width = 800, int height = 600, AppSettings settings = {.title="Game"}) : App(width, height, settings) {
            RegisterSystem<BehaviourSystem>();
            RegisterSystem<PhysicSystem>();
            RegisterSystem<SpriteRenderer>();
            RegisterSystem<UIRenderer>();
            RegisterSystem<ParticleSystem>();
            
            settings.clearColor = RGBAColor(92, 102, 86);
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
    
    Game ogl_engine(1600, 900, settings);
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