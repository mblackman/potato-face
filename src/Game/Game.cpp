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
#include "../Events/KeyInputEvent.h"
#include "../General/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/DrawColliderSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() : window_(nullptr),
               renderer_(nullptr),
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
    registry_->AddSystem<DamageSystem>();

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
    registry_->GetSystem<MovementSystem>().SubscribeToEvents(event_bus_);
    SubscribeToEvents(event_bus_);

    // Calculate delta time
    double deltaTime = (SDL_GetTicks() - milliseconds_previous_frame_) / 1000.0;

    milliseconds_previous_frame_ = SDL_GetTicks();

    registry_->GetSystem<MovementSystem>().Update(deltaTime);
    registry_->GetSystem<AnimationSystem>().Update();
    registry_->GetSystem<CollisionSystem>().Update(event_bus_);
    registry_->GetSystem<DamageSystem>().Update();
    registry_->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);
    SDL_RenderClear(renderer_);

    // TODO: Render with ECS
    registry_->GetSystem<RenderSystem>().Update(renderer_, asset_manager_);

    if (show_colliders_) {
        registry_->GetSystem<DrawColliderSystem>().Update(renderer_);
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
        case InputKey::Key_Esc:
            is_running_ = false;
            break;
        case InputKey::Key_F5:
            show_colliders_ = !show_colliders_;
            break;
        default:
            break;
    }
}

KeyInputEvent Game::GetKeyInputEvent(SDL_KeyboardEvent* event) {
    auto inputKey = GetInputKey(event->keysym.sym);
    auto inputModifier = GetModifierKey(event->keysym.mod);
    bool isPressed = event->state == SDL_PRESSED;
    std::string keyString = std::to_string(event->keysym.sym);

    return KeyInputEvent(inputKey, inputModifier, isPressed, keyString);
}

InputKey Game::GetInputKey(SDL_Keycode sdlKeyCode) {
    switch (sdlKeyCode) {
        case SDLK_a:
            return InputKey::Key_A;
        case SDLK_b:
            return InputKey::Key_B;
        case SDLK_c:
            return InputKey::Key_C;
        case SDLK_d:
            return InputKey::Key_D;
        case SDLK_e:
            return InputKey::Key_E;
        case SDLK_f:
            return InputKey::Key_F;
        case SDLK_g:
            return InputKey::Key_G;
        case SDLK_h:
            return InputKey::Key_H;
        case SDLK_i:
            return InputKey::Key_I;
        case SDLK_j:
            return InputKey::Key_J;
        case SDLK_k:
            return InputKey::Key_K;
        case SDLK_l:
            return InputKey::Key_L;
        case SDLK_m:
            return InputKey::Key_M;
        case SDLK_n:
            return InputKey::Key_N;
        case SDLK_o:
            return InputKey::Key_O;
        case SDLK_p:
            return InputKey::Key_P;
        case SDLK_q:
            return InputKey::Key_Q;
        case SDLK_r:
            return InputKey::Key_R;
        case SDLK_s:
            return InputKey::Key_S;
        case SDLK_t:
            return InputKey::Key_T;
        case SDLK_u:
            return InputKey::Key_U;
        case SDLK_v:
            return InputKey::Key_V;
        case SDLK_w:
            return InputKey::Key_W;
        case SDLK_x:
            return InputKey::Key_X;
        case SDLK_y:
            return InputKey::Key_Y;
        case SDLK_z:
            return InputKey::Key_Z;
        case SDLK_0:
            return InputKey::Key_Num0;
        case SDLK_1:
            return InputKey::Key_Num1;
        case SDLK_2:
            return InputKey::Key_Num2;
        case SDLK_3:
            return InputKey::Key_Num3;
        case SDLK_4:
            return InputKey::Key_Num4;
        case SDLK_5:
            return InputKey::Key_Num5;
        case SDLK_6:
            return InputKey::Key_Num6;
        case SDLK_7:
            return InputKey::Key_Num7;
        case SDLK_8:
            return InputKey::Key_Num8;
        case SDLK_9:
            return InputKey::Key_Num9;
        case SDLK_F1:
            return InputKey::Key_F1;
        case SDLK_F2:
            return InputKey::Key_F2;
        case SDLK_F3:
            return InputKey::Key_F3;
        case SDLK_F4:
            return InputKey::Key_F4;
        case SDLK_F5:
            return InputKey::Key_F5;
        case SDLK_F6:
            return InputKey::Key_F6;
        case SDLK_F7:
            return InputKey::Key_F7;
        case SDLK_F8:
            return InputKey::Key_F8;
        case SDLK_F9:
            return InputKey::Key_F9;
        case SDLK_F10:
            return InputKey::Key_F10;
        case SDLK_F11:
            return InputKey::Key_F11;
        case SDLK_F12:
            return InputKey::Key_F12;
        case SDLK_BACKSPACE:
            return InputKey::Key_Backspace;
        case SDLK_TAB:
            return InputKey::Key_Tab;
        case SDLK_RETURN:
            return InputKey::Key_Enter;  // SDLK_RETURN is '\r' (carriage return)
        case SDLK_SPACE:
            return InputKey::Key_Space;
        case SDLK_ESCAPE:
            return InputKey::Key_Esc;
        case SDLK_QUOTE:
            return InputKey::Key_Apostrophe;
        case SDLK_COMMA:
            return InputKey::Key_Comma;
        case SDLK_MINUS:
            return InputKey::Key_Minus;
        case SDLK_PERIOD:
            return InputKey::Key_Period;
        case SDLK_SLASH:
            return InputKey::Key_Slash;
        case SDLK_SEMICOLON:
            return InputKey::Key_Semicolon;
        case SDLK_EQUALS:
            return InputKey::Key_Equals;
        case SDLK_LEFTBRACKET:
            return InputKey::Key_LeftBracket;
        case SDLK_BACKSLASH:
            return InputKey::Key_Backslash;
        case SDLK_RIGHTBRACKET:
            return InputKey::Key_RightBracket;
        case SDLK_BACKQUOTE:
            return InputKey::Key_GraveAccent;
        case SDLK_CAPSLOCK:
            return InputKey::Key_CapsLock;
        case SDLK_PRINTSCREEN:
            return InputKey::Key_PrintScreen;
        case SDLK_SCROLLLOCK:
            return InputKey::Key_ScrollLock;
        case SDLK_PAUSE:
            return InputKey::Key_Pause;
        case SDLK_INSERT:
            return InputKey::Key_Insert;
        case SDLK_HOME:
            return InputKey::Key_Home;
        case SDLK_PAGEUP:
            return InputKey::Key_PageUp;
        case SDLK_DELETE:
            return InputKey::Key_Delete;
        case SDLK_END:
            return InputKey::Key_End;
        case SDLK_PAGEDOWN:
            return InputKey::Key_PageDown;
        case SDLK_RIGHT:
            return InputKey::Key_Right;
        case SDLK_LEFT:
            return InputKey::Key_Left;
        case SDLK_DOWN:
            return InputKey::Key_Down;
        case SDLK_UP:
            return InputKey::Key_Up;
        case SDLK_NUMLOCKCLEAR:
            return InputKey::Key_NumLock;
        case SDLK_KP_DIVIDE:
            return InputKey::Key_KeypadDivide;
        case SDLK_KP_MULTIPLY:
            return InputKey::Key_KeypadMultiply;
        case SDLK_KP_MINUS:
            return InputKey::Key_KeypadMinus;
        case SDLK_KP_PLUS:
            return InputKey::Key_KeypadPlus;
        case SDLK_KP_ENTER:
            return InputKey::Key_KeypadEnter;
        case SDLK_KP_1:
            return InputKey::Key_Keypad1;
        case SDLK_KP_2:
            return InputKey::Key_Keypad2;
        case SDLK_KP_3:
            return InputKey::Key_Keypad3;
        case SDLK_KP_4:
            return InputKey::Key_Keypad4;
        case SDLK_KP_5:
            return InputKey::Key_Keypad5;
        case SDLK_KP_6:
            return InputKey::Key_Keypad6;
        case SDLK_KP_7:
            return InputKey::Key_Keypad7;
        case SDLK_KP_8:
            return InputKey::Key_Keypad8;
        case SDLK_KP_9:
            return InputKey::Key_Keypad9;
        case SDLK_KP_0:
            return InputKey::Key_Keypad0;
        case SDLK_KP_PERIOD:
            return InputKey::Key_KeypadDecimal;
        case SDLK_LCTRL:
            return InputKey::Key_LeftControl;
        case SDLK_LSHIFT:
            return InputKey::Key_LeftShift;
        case SDLK_LALT:
            return InputKey::Key_LeftAlt;
        case SDLK_RCTRL:
            return InputKey::Key_RightControl;
        case SDLK_RSHIFT:
            return InputKey::Key_RightShift;
        case SDLK_RALT:
            return InputKey::Key_RightAlt;
        default:
            return InputKey::Key_Unknown;
    }
}

InputModifier Game::GetModifierKey(Uint16 mod) {
    auto modifier = InputModifier::Mod_None;

    if (mod & KMOD_LSHIFT) {
        modifier = static_cast<InputModifier>(static_cast<unsigned int>(modifier) | static_cast<unsigned int>(InputModifier::Mod_LeftShift));
    }

    if (mod & KMOD_RSHIFT) {
        modifier = static_cast<InputModifier>(static_cast<unsigned int>(modifier) | static_cast<unsigned int>(InputModifier::Mod_RightShift));
    }

    if (mod & KMOD_LCTRL) {
        modifier = static_cast<InputModifier>(static_cast<unsigned int>(modifier) | static_cast<unsigned int>(InputModifier::Mod_LeftControl));
    }

    if (mod & KMOD_RCTRL) {
        modifier = static_cast<InputModifier>(static_cast<unsigned int>(modifier) | static_cast<unsigned int>(InputModifier::Mod_RightControl));
    }

    if (mod & KMOD_LALT) {
        modifier = static_cast<InputModifier>(static_cast<unsigned int>(modifier) | static_cast<unsigned int>(InputModifier::Mod_LeftAlt));
    }

    if (mod & KMOD_RALT) {
        modifier = static_cast<InputModifier>(static_cast<unsigned int>(modifier) | static_cast<unsigned int>(InputModifier::Mod_RightAlt));
    }

    return modifier;
}