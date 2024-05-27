#pragma once

#include <SDL2/SDL.h>

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

    SDL_Window* _window;
    SDL_Renderer* _renderer;
    bool _isRunning;
};
