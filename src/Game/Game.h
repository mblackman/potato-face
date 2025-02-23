#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <sol/sol.hpp>

#include "../AssetManager/AssetManager.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyInputEvent.h"

const int kFps = 60;
const int kMillisecondsPerFrame = 1000 / kFps;

class Game {
   public:
    Game();
    ~Game();

    void Initialize();
    void Destroy();
    void Run();

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;

   private:
    void ProcessInput();
    void Update();
    void Render();
    void Setup();
    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
    void OnKeyInputEvent(KeyInputEvent& event);
    KeyInputEvent GetKeyInputEvent(SDL_KeyboardEvent* event);

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Rect camera_;
    bool is_running_;
    bool show_colliders_;
    int milliseconds_previous_frame_ = 0;

    sol::state lua;
    std::unique_ptr<Registry> registry_;
    std::unique_ptr<AssetManager> asset_manager_;
    std::unique_ptr<EventBus> event_bus_;
};
