-- Define a table with the values of the first level
Level = {
    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] =
        { type = "texture", id = "tilemap-texture", file = "./assets/tilemaps/jungle.png" },
        { type = "texture", id = "chopper-texture", file = "./assets/images/chopper-spritesheet.png" },
        { type = "texture", id = "tank-texture",    file = "./assets/images/tank-tiger-up.png" },
        { type = "texture", id = "bullet-texture",  file = "./assets/images/bullet.png" },
        { type = "font"   , id = "arial-font-5",    file = "./assets/fonts/arial.ttf", font_size = 5 },
        { type = "font"   , id = "arial-font-10",   file = "./assets/fonts/arial.ttf", font_size = 10 }
    },

    ----------------------------------------------------
    -- table to define the map config variables
    ----------------------------------------------------
    tilemap = {
        map_file = "./assets/tilemaps/jungle.map",
        texture_asset_id = "tilemap-texture",
        num_cols = 10,
        tile_size = 32,
        scale = 2.0
    },

    ----------------------------------------------------
    -- table to define entities and their components
    ----------------------------------------------------
    entities = {
        [0] =
        {
            -- Player
            tag = "player",
            components = {
                transform = {
                    position = { x = 242, y = 110 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "chopper-texture",
                    width = 32,
                    height = 32,
                    z_index = 4,
                    fixed = false,
                    src_rect_x = 0,
                    src_rect_y = 0
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 32,
                    height = 25,
                    offset = { x = 0, y = 5 }
                },
                health = {
                    max_health = 100
                },
                projectile_emitter = {
                    projectile_velocity = { x = 200, y = 200 },
                    projectile_duration = 10, -- seconds
                    repeat_frequency = 0, -- seconds
                    hit_damage = 10,
                    friendly = true
                },
                keyboard_controller = {
                    velocity = 80
                },
                camera_follow = {
                    follow = true
                }
            }
        },
        {
            -- Tank
            group = "enemies",
            components = {
                transform = {
                    position = { x = 200, y = 497 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "tank-texture",
                    width = 32,
                    height = 32,
                    z_index = 2
                },
                boxcollider = {
                    width = 25,
                    height = 18,
                    offset = { x = 0, y = 7 }
                },
                health = {
                    max_health = 100
                },
                projectile_emitter = {
                    projectile_velocity = { x = 100, y = 0 },
                    projectile_duration = 2, -- seconds
                    repeat_frequency = 1, -- seconds
                    hit_damage = 20,
                    friendly = false
                }
            }
        }
    }
}