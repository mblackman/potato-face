edit_grid_num_tiles_wide = 20
edit_grid_num_tiles_high = 10
select_grid_num_tiles_wide = 10
select_grid_num_tiles_high = 3

grid_texture_height = 32
grid_texture_width = 32

edit_grid_width = edit_grid_num_tiles_wide * grid_texture_width
edit_grid_height = edit_grid_num_tiles_high * grid_texture_height

edit_grid_scale = game_window_width / edit_grid_width

document = {    
    assets = {
        [0] =
        { type = "texture", id = "tilemap-texture", file = "./assets/tilemaps/jungle.png" },
        { type = "font"   , id = "arial-font-5",    file = "./assets/fonts/arial.ttf", font_size = 5 },
        { type = "font"   , id = "arial-font-10",   file = "./assets/fonts/arial.ttf", font_size = 10 }
    },
    entities = {}  
}

index = 0

-- Tile map
for i=0, edit_grid_num_tiles_high-1 do
    for j=0, edit_grid_num_tiles_wide-1 do
        x_position = j * grid_texture_width * edit_grid_scale
        y_position = i * grid_texture_height * edit_grid_scale
        document.entities[index] = {
            components = {
                transform = {
                    position = { x = x_position, y = y_position },
                    scale = { x = edit_grid_scale, y = edit_grid_scale },
                    rotation = 0.0,
                },
                sprite = {
                    texture_asset_id = "tilemap-texture",
                    width = grid_texture_height,
                    height = grid_texture_width,
                    z_index = 1,
                    fixed = true,
                    src_rect_x = 0,
                    src_rect_y = 0
                },
                boxcollider = {
                    width = grid_texture_height,
                    height = grid_texture_width,
                    offset = { x = 0, y = 0 }
                },
                button = {
                    is_active = true,
                    my_index = index,
                    on_click_script = {
                        [0] = 
                        function(self, entity)
                            print("Clicked tile with index "..self.my_index)
                        end
                    }
                }
            }
        }

        index = index + 1
    end
end

-- Background
document.entities[index] = {
    components = {
        transform = {
            position = { x = 0, y = edit_grid_height * edit_grid_scale },
            scale = { x = 1.0, y = 1.0 },
            rotation = 0.0,
        },
        square = {
            position = { x = 0, y = edit_grid_height * edit_grid_scale },
            width = game_window_width,
            height = game_window_height - edit_grid_height * edit_grid_scale,
            color = {r = 218, g = 112, b = 214},
            is_fixed = true
        }
    }
}

index = index + 1

-- Map sprite
document.entities[index] = {
    transform = {
        position = { x = 0, y = game_window_height - 96 },
        scale = { x = edit_grid_scale, y = edit_grid_scale },
        rotation = 0.0,
    },
    sprite = {
        texture_asset_id = "tilemap-texture",
        width = 320,
        height = 96,
        z_index = 2,
        fixed = true,
        src_rect_x = 0,
        src_rect_y = 0
    }
}