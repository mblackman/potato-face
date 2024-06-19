#include "Game.h"
#include "../ECS/ECS.h"

#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../General/Logger.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() : window_(nullptr),
               renderer_(nullptr),
               is_running_(false),
               milliseconds_previous_frame_() {
    Logger::Info("Game Constructor called.");
    registry_ = std::make_unique<Registry>();
    asset_manager_ = std::make_unique<AssetManager>();
}

Game::~Game() {
    Logger::Info("Game Destructor called.");
}

void Game::Initialize() {
    auto result = SDL_Init(SDL_INIT_EVERYTHING);

    if (result != 0) {
        Logger::Error("SDL_Init Error: " + std::string(SDL_GetError()));
        return;
    }

    window_ = SDL_CreateWindow(
        "Potato Face",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);

    if (!window_) {
        Logger::Error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
        return;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, 0);

    if (!renderer_) {
        Logger::Error("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
        return;
    }

    is_running_ = true;
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Game::Run() {
    Setup();

    while (is_running_) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Setup() {
    // Add systems
    registry_->AddSystem<MovementSystem>();
    registry_->AddSystem<RenderSystem>();

    // Add assets to asset manager
    asset_manager_->AddTexture(renderer_, "tank-image", "./assets/images/tank-panther-right.png");
    asset_manager_->AddTexture(renderer_, "truck-image", "./assets/images/truck-ford-right.png");

    // Create an entity for the tank
    auto tank = registry_->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10, 30), glm::vec2(3, 3), 45.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(10, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

    // Create an entity for the truck
    auto truck = registry_->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50, 100), glm::vec2(1, 1), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0, 50));
    truck.AddComponent<SpriteComponent>("truck-image", 10, 50);
}

void Game::ProcessInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running_ = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running_ = false;
                }
                break;

            default:
                break;
        }
    }
}

void Game::Update() {
    // If we are too fast, waste some time until we reach the frame time
    int timeToWait = kMillisecondsPerFrame - (SDL_GetTicks() - milliseconds_previous_frame_);
    if (timeToWait > 0 && timeToWait <= kMillisecondsPerFrame) {
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - milliseconds_previous_frame_) / 1000.0;

    milliseconds_previous_frame_ = SDL_GetTicks();

    registry_->GetSystem<MovementSystem>().Update(deltaTime);

    registry_->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);
    SDL_RenderClear(renderer_);

    // TODO: Render with ECS
    registry_->GetSystem<RenderSystem>().Update(renderer_, asset_manager_);

    SDL_RenderPresent(renderer_);
}
