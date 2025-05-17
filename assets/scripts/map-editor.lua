edit_grid_num_tiles_wide = 20
edit_grid_num_tiles_high = 10
select_grid_num_tiles_wide = 10
select_grid_num_tiles_high = 3

grid_texture_height = 32
grid_texture_width = 32

edit_grid_width = edit_grid_num_tiles_wide * grid_texture_width
edit_grid_height = edit_grid_num_tiles_high * grid_texture_height

edit_grid_scale = game_window_width / edit_grid_width

selected_tile_index = 0

document = {    
    assets = {
        [0] =
        { type = "texture", id = "tilemap-texture", file = "./assets/tilemaps/jungle.png" },
        { type = "font"   , id = "arial-font-5",    file = "./assets/fonts/arial.ttf", font_size = 5 },
        { type = "font"   , id = "arial-font-10",   file = "./assets/fonts/arial.ttf", font_size = 10 }
    },
    entities = {},
    components = {
        on_update_script = {
            [0] = 
            function(entity, delta_time, elapsed_time) 
                if is_key_pressed("w") and is_key_held("ctrl") then
                    print("Ctrl+W key pressed")
                end
            end
        }
    }
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
                    layer = 1,
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
                            if selected_tile_index ~= nil then
                                local tile_index = self.my_index
                                local src_rect_x = selected_tile_index % select_grid_num_tiles_wide * grid_texture_width
                                local src_rect_y = math.floor(selected_tile_index / select_grid_num_tiles_wide) * grid_texture_height

                                -- Update the sprite texture
                                set_sprite_src_rect(entity, src_rect_x, src_rect_y)

                                -- Update the tilemap file
                                print("Updating tilemap file with new tile index "..selected_tile_index)
                                print("src_rect_x: "..src_rect_x)
                                print("src_rect_y: "..src_rect_y)
                            end
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
select_grid_start_height = game_window_height - select_grid_num_tiles_high * grid_texture_height * edit_grid_scale

for i=0, select_grid_num_tiles_high-1 do
    for j=0, select_grid_num_tiles_wide-1 do
        x_position = j * grid_texture_width * edit_grid_scale
        y_position = i * grid_texture_height * edit_grid_scale + select_grid_start_height
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
                    layer = 5,
                    fixed = true,
                    src_rect_x = j * 32,
                    src_rect_y = i * 32
                },
                boxcollider = {
                    width = grid_texture_height,
                    height = grid_texture_width,
                    offset = { x = 0, y = 0 }
                },
                button = {
                    is_active = true,
                    tile_index = j + i * select_grid_num_tiles_wide,
                    on_click_script = {
                        [0] = 
                        function(self, entity)
                            print("Clicked selector with tile index "..self.tile_index)
                            selected_tile_index = self.tile_index
                        end
                    }
                }
            }
        }

        index = index + 1
    end
end