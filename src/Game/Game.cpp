#include "Game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../General/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DrawColliderSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() : window_(nullptr),
               renderer_(nullptr),
               is_running_(false),
               milliseconds_previous_frame_() {
    registry_ = std::make_unique<Registry>();
    asset_manager_ = std::make_unique<AssetManager>();
    event_bus_ = std::make_unique<EventBus>();
    Logger::Info("Game Constructor called.");
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

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

    window_ = SDL_CreateWindow(
        "Potato Face",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS);

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

void Game::LoadLevel(int level) {
    // Add systems
    registry_->AddSystem<MovementSystem>();
    registry_->AddSystem<RenderSystem>();
    registry_->AddSystem<AnimationSystem>();
    registry_->AddSystem<CollisionSystem>();
    registry_->AddSystem<DrawColliderSystem>();

    // Add assets to asset manager
    asset_manager_->AddTexture(renderer_, "tank-image", "./assets/images/tank-panther-right.png");
    asset_manager_->AddTexture(renderer_, "truck-image", "./assets/images/truck-ford-right.png");
    asset_manager_->AddTexture(renderer_, "chopper-image", "./assets/images/chopper.png");
    asset_manager_->AddTexture(renderer_, "radar-image", "./assets/images/radar.png");

    // Load Tilemap
    asset_manager_->AddTexture(renderer_, "jungle-tile-map", "./assets/tilemaps/jungle.png");

    std::ifstream file("./assets/tilemaps/jungle.map");
    std::string line;
    int tileMapColumns = 10;
    int rowNumber = 0;
    int columnNumber = 0;
    int tileWidth = 32;
    int tileHeight = 32;
    double tileMapScale = 2.0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<std::string> row;
        columnNumber = 0;

        while (std::getline(ss, value, ',')) {
            row.push_back(value);
        }

        for (const auto& item : row) {
            int value = std::stoi(item);
            int rowIndex = value / tileMapColumns;
            int columnIndex = value % tileMapColumns;

            auto tile = registry_->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(tileWidth * columnNumber * tileMapScale, tileHeight * rowNumber * tileMapScale), glm::vec2(tileMapScale, tileMapScale), 0.0);
            tile.AddComponent<SpriteComponent>("jungle-tile-map", tileWidth, tileHeight, 0, tileWidth * columnIndex, tileHeight * rowIndex);
            columnNumber++;
        }

        rowNumber++;
    }

    // Chopper
    auto chopper = registry_->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(10, 30), glm::vec2(1, 1), 45.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0, 0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 15, true);

    // Radar
    auto radar = registry_->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10), glm::vec2(1, 1), 45.0);
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    // Create an entity for the tank
    auto tank = registry_->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(500, 10), glm::vec2(3, 3), 180);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-50, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    tank.AddComponent<BoxColliderComponent>(32, 32);

    // Create an entity for the truck
    auto truck = registry_->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10, 10), glm::vec2(3, 3), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
}

void Game::Setup() {
    LoadLevel(1);
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
    registry_->GetSystem<AnimationSystem>().Update();
    registry_->GetSystem<CollisionSystem>().Update();
    registry_->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);
    SDL_RenderClear(renderer_);

    // TODO: Render with ECS
    registry_->GetSystem<RenderSystem>().Update(renderer_, asset_manager_);
    registry_->GetSystem<DrawColliderSystem>().Update(renderer_);

    SDL_RenderPresent(renderer_);
}
