#include "Game.h"
#include "../ECS/ECS.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../General/Logger.h"

Game::Game() : _window(nullptr),
               _renderer(nullptr),
               _isRunning(false),
               _millisecondsPreviousFrame() {
    Logger::Info("Game Constructor called.");
    _registry = new Registry();
}

Game::~Game() {
    Logger::Info("Game Destructor called.");
}

void Game::Initialize() {
    auto result = SDL_Init(SDL_INIT_EVERYTHING);

    if (result != 0) {
        Logger::Err("SDL_Init Error: " + std::string(SDL_GetError()));
        return;
    }

    _window = SDL_CreateWindow(
        "Potato Face",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);

    if (!_window) {
        Logger::Err("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
        return;
    }

    _renderer = SDL_CreateRenderer(_window, -1, 0);

    if (!_renderer) {
        Logger::Err("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
        return;
    }

    _isRunning = true;
}

void Game::Destroy() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Game::Run() {
    Setup();

    while (_isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Setup() {
    // Create an entity for the tank
    auto tank = _registry->CreateEntity();
}

void Game::ProcessInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                _isRunning = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    _isRunning = false;
                }
                break;

            default:
                break;
        }
    }
}

void Game::Update() {
    // If we are too fast, waste some time until we reach the frame time
    int timeToWait = kMillisecondsPerFrame - (SDL_GetTicks() - _millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= kMillisecondsPerFrame) {
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - _millisecondsPreviousFrame) / 1000.0;

    _millisecondsPreviousFrame = SDL_GetTicks();
    // MovementSystem::Update(deltaTime);
}

void Game::Render() {
    SDL_SetRenderDrawColor(_renderer, 21, 21, 21, 255);
    SDL_RenderClear(_renderer);

    // TODO: Render with ECS

    SDL_RenderPresent(_renderer);
}
