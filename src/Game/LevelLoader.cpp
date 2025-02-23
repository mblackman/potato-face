#include "LevelLoader.h"

#include <fstream>
#include <sstream>
#include <string>

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
#include "../Game/Game.h"
#include "../General/Logger.h"

LevelLoader::LevelLoader() {
    Logger::Info("Level Loader created.");
}

LevelLoader::~LevelLoader() {
    Logger::Info("Level Loader destroyed.");
}

void LevelLoader::LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetManager>& assetManager, SDL_Renderer* renderer, int levelNumber) {
    auto filePath = "./assets/scripts/level" + std::to_string(levelNumber) + ".lua";
    sol::load_result script = lua.load_file(filePath);

    if (!script.valid()) {
        sol::error error = script;
        Logger::Error("Level script is not valid: " + std::string(error.what()));
        return;
    }

    lua.script_file(filePath);
    Logger::Info("Just opened level1.lua");

    sol::table level = lua["Level"];

    // Read level assets
    sol::table assets = level["assets"];
    int i = 0;

    while (true) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (!hasAsset) {
            break;
        }

        sol::table asset = assets[i];
        std::string assetType = asset["type"];

        if (assetType == "texture") {
            assetManager->AddTexture(renderer, asset["id"], asset["file"]);
        } else if (assetType == "font") {
            int fontSize = asset["font_size"];
            assetManager->AddFont(asset["id"], asset["file"], fontSize);
        } else {
            Logger::Error("Unknown asset type: " + assetType);
        }

        i++;
    }

    // Load the tilemap
    sol::table tileMap = level["tilemap"];
    std::string mapFile = tileMap["map_file"];
    std::ifstream file(mapFile);
    std::string line;
    int tileMapColumns = tileMap["num_cols"];
    std::string tileMapTextureId = tileMap["texture_asset_id"];
    int rowNumber = 0;
    int columnNumber = 0;
    int tileWidth = tileMap["tile_size"];
    int tileHeight = tileMap["tile_size"];
    double tileMapScale = tileMap["scale"];

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

            auto tile = registry->CreateEntity();
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(tileWidth * columnNumber * tileMapScale, tileHeight * rowNumber * tileMapScale), glm::vec2(tileMapScale, tileMapScale), 0.0);
            tile.AddComponent<SpriteComponent>(tileMapTextureId, tileWidth, tileHeight, 0, false, tileWidth * columnIndex, tileHeight * rowIndex);
            columnNumber++;
        }

        rowNumber++;
    }
    file.close();

    Game::mapWidth = columnNumber * tileWidth * tileMapScale;
    Game::mapHeight = rowNumber * tileHeight * tileMapScale;

    // Create entities
    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];

        Entity newEntity = registry->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)),
                    entity["components"]["transform"]["rotation"].get_or(0.0));
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)));
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false),
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0));
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1));
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)));
            }

            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["max_health"].get_or(100)));
            }

            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false));
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                double velocity = entity["components"]["keyboard_controller"]["velocity"];
                newEntity.AddComponent<KeyboardControlComponent>(velocity);
            }
        }
        i++;
    }
}