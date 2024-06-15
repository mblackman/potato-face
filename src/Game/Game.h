#pragma once

#include <memory>

#include <SDL2/SDL.h>

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

   private:
    void ProcessInput();
    void Update();
    void Render();
    void Setup();

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool is_running_;
    int milliseconds_previous_frame_ = 0;
    std::unique_ptr<Registry> registry_;
};
