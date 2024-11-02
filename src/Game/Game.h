#pragma once

#include <SDL2/SDL.h>

#include <memory>

#include "../AssetManager/AssetManager.h"
#include "../ECS/ECS.h"

const int kFps = 60;
const int kMillisecondsPerFrame = 1000 / kFps;

class Game {
   public:
    Game();
    ~Game();

    void Initialize();
    void Destroy();
    void Run();

    int windowWidth;
    int windowHeight;

   private:
    void ProcessInput();
    void Update();
    void Render();
    void Setup();
    void LoadLevel(int level);

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool is_running_;
    int milliseconds_previous_frame_ = 0;
    std::unique_ptr<Registry> registry_;
    std::unique_ptr<AssetManager> asset_manager_;
};
