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
#include "../Components/CameraFollowComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Events/KeyInputEvent.h"
#include "../General/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CameraFollowSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/DisplayHealthSystem.h"
#include "../Systems/DrawColliderSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderPrimitiveSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/RenderTextSystem.h"

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() : window_(nullptr),
               renderer_(nullptr),
               camera_(),
               is_running_(false),
               show_colliders_(false),
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

    if (TTF_Init() != 0) {
        Logger::Error("TTF_Init Error: " + std::string(TTF_GetError()));
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

    camera_.x = 0;
    camera_.y = 0;
    camera_.w = windowWidth;
    camera_.h = windowHeight;

    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);

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
    registry_->AddSystem<RenderTextSystem>();
    registry_->AddSystem<RenderPrimitiveSystem>();
    registry_->AddSystem<AnimationSystem>();
    registry_->AddSystem<CollisionSystem>();
    registry_->AddSystem<DrawColliderSystem>();
    registry_->AddSystem<DamageSystem>();
    registry_->AddSystem<KeyboardControlSystem>();
    registry_->AddSystem<CameraFollowSystem>();
    registry_->AddSystem<ProjectileEmitSystem>();
    registry_->AddSystem<ProjectileLifecycleSystem>();
    registry_->AddSystem<DisplayHealthSystem>();

    // Add assets to asset manager
    asset_manager_->AddTexture(renderer_, "tank-image", "./assets/images/tank-panther-right.png");
    asset_manager_->AddTexture(renderer_, "truck-image", "./assets/images/truck-ford-right.png");
    asset_manager_->AddTexture(renderer_, "chopper-image", "./assets/images/chopper-spritesheet.png");
    asset_manager_->AddTexture(renderer_, "radar-image", "./assets/images/radar.png");
    asset_manager_->AddTexture(renderer_, "bullet-image", "./assets/images/bullet.png");

    asset_manager_->AddFont("arial-font", "./assets/fonts/arial.ttf", 20);
    asset_manager_->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 20);

    // Load Tilemap
    asset_manager_->AddTexture(renderer_, "jungle-tile-map", "./assets/tilemaps/jungle.png");

    std::ifstream file("./assets/tilemaps/jungle.map");
    std::string line;
    int tileMapColumns = 10;
    int rowNumber = 0;
    int columnNumber = 0;
    int tileWidth = 32;
    int tileHeight = 32;
    double tileMapScale = 4.0;

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
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(tileWidth * columnNumber * tileMapScale, tileHeight * rowNumber * tileMapScale), glm::vec2(tileMapScale, tileMapScale), 0.0);
            tile.AddComponent<SpriteComponent>("jungle-tile-map", tileWidth, tileHeight, 0, false, tileWidth * columnIndex, tileHeight * rowIndex);
            columnNumber++;
        }

        rowNumber++;
    }
    file.close();

    mapWidth = columnNumber * tileWidth * tileMapScale;
    mapHeight = rowNumber * tileHeight * tileMapScale;

    // Create entities
    // Chopper
    auto chopper = registry_->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(10, 30), glm::vec2(3, 3), 0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0, 0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<KeyboardControlComponent>(250.0);
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150, 150), 5000, 0, 100, true);
    chopper.AddComponent<HealthComponent>(100);

    //  Radar
    auto radar = registry_->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10), glm::vec2(1, 1), 45.0);
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    // Create an entity for the tank
    auto tank = registry_->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(500, 10), glm::vec2(3, 3), 180);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-50, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    tank.AddComponent<BoxColliderComponent>(32, 32);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(-100, 0), 10000, 5000, 33, false);
    tank.AddComponent<HealthComponent>(100);

    // Create an entity for the truck
    auto truck = registry_->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(10, 10), glm::vec2(3, 3), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(100, 0), 10000, 2000, 33, false);
    truck.AddComponent<HealthComponent>(100);
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
            case SDL_KEYUP: {
                KeyInputEvent keyInputEvent = GetKeyInputEvent(&event.key);
                event_bus_->EmitEvent<KeyInputEvent>(keyInputEvent);
                break;
            }
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

    // Subscribe to events
    event_bus_->Reset();
    registry_->GetSystem<DamageSystem>().SubscribeToEvents(event_bus_);
    registry_->GetSystem<KeyboardControlSystem>().SubscribeToEvents(event_bus_);
    registry_->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(event_bus_);
    SubscribeToEvents(event_bus_);

    // Calculate delta time
    double deltaTime = (SDL_GetTicks() - milliseconds_previous_frame_) / 1000.0;

    milliseconds_previous_frame_ = SDL_GetTicks();

    registry_->GetSystem<MovementSystem>().Update(deltaTime);
    registry_->GetSystem<AnimationSystem>().Update();
    registry_->GetSystem<CollisionSystem>().Update(event_bus_);
    registry_->GetSystem<DamageSystem>().Update();
    registry_->GetSystem<KeyboardControlSystem>().Update();
    registry_->GetSystem<CameraFollowSystem>().Update(camera_);
    registry_->GetSystem<ProjectileEmitSystem>().Update(registry_);
    registry_->GetSystem<ProjectileLifecycleSystem>().Update();
    registry_->GetSystem<DisplayHealthSystem>().Update(registry_);
    registry_->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);
    SDL_RenderClear(renderer_);

    // TODO: Render with ECS
    registry_->GetSystem<RenderSystem>().Update(renderer_, asset_manager_, camera_);
    registry_->GetSystem<RenderTextSystem>().Update(renderer_, asset_manager_, camera_);
    registry_->GetSystem<RenderPrimitiveSystem>().Update(renderer_, camera_);

    if (show_colliders_) {
        registry_->GetSystem<DrawColliderSystem>().Update(renderer_, camera_);
    }

    SDL_RenderPresent(renderer_);
}

void Game::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
    eventBus->SubscribeEvent<Game, KeyInputEvent>(this, &Game::OnKeyInputEvent);
}

void Game::OnKeyInputEvent(KeyInputEvent& event) {
    if (!event.isPressed) {
        return;
    }

    switch (event.inputKey) {
        case SDLK_ESCAPE:
            is_running_ = false;
            break;
        case SDLK_F5:
            show_colliders_ = !show_colliders_;
            break;
        default:
            break;
    }
}

KeyInputEvent Game::GetKeyInputEvent(SDL_KeyboardEvent* event) {
    bool isPressed = event->state == SDL_PRESSED;

    return KeyInputEvent(
        event->keysym.sym,
        static_cast<SDL_Keymod>(event->keysym.mod),
        isPressed);
}
