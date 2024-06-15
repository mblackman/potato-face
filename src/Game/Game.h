#pragma once

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

    SDL_Window* _window;
    SDL_Renderer* _renderer;
    bool _isRunning;
    int _millisecondsPreviousFrame = 0;
    Registry* _registry;
};
