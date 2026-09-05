#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "global.hpp"
#include "utils.hpp"
#include "red-ghost.hpp"
#include "map-collision.hpp"
#include "asset-path.hpp"

RedGhost::RedGhost():
    direction{0},
    use_door{0},
    current_sprite_frame_edge{0},
    ghost_texture(sf::Texture::loadFromFile(asset_path("./assets/sprite_sheets/red_ghost.png")).value()),
    frightened_move_lag{GHOST_FRIGHTENED_MOVE_LAG}
{}

void RedGhost::draw(sf::RenderTarget &i_window, sf::Clock &animation_clock, const MovementMode &cur_movement_mode)
{
    if(cur_movement_mode == MovementMode::Frightened_mode)  current_sprite_frame_edge = GHOST_FRIGHTENED_FRAME_END ;

    sf::Rect2f rectSourceSprite({static_cast<float>(current_sprite_frame_edge), 0.f}, {24.f, 24.f});  // width = 24 , height = 24  // width = 24 , height = 24  
    ghost_sprite.textureRect = rectSourceSprite;
    ghost_sprite.scale = {0.65f, 0.65f};
    ghost_sprite.position = {static_cast<float>(position.x), static_cast<float>(position.y)};

    // After a specified duration we change the sprite section currently in view
 if (animation_clock.getElapsedTime().asSeconds() > GHOST_FRAME_SWITCH_DURATION && cur_movement_mode != MovementMode::Frightened_mode)
    {
        if(rectSourceSprite.position.x == current_sprite_frame_edge)
        {
            rectSourceSprite.position.x =  current_sprite_frame_edge - SPRITE_GAME_CHARACTER_WIDTH;
        }
        else{
            rectSourceSprite.position.x = current_sprite_frame_edge;
        }

        ghost_sprite.textureRect = rectSourceSprite;
        animation_clock.restart();
    }

    i_window.draw(ghost_sprite, {.texture = &ghost_texture});
}

void RedGhost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void RedGhost::set_home(short i_x,short i_y)
{
    home = {i_x,i_y};
}

void RedGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman,MovementMode &cur_movement_mode)
{
    // Check for collision with pacman
    if(sprite_collision(ghost_sprite,i_pacman.get_pacman_sprite()))
    {
        if(cur_movement_mode == MovementMode::Frightened_mode)
        {
            reset();
        }
        else
        {
            i_pacman.reduce_life();
        }
    }

    // Check for collision in all directions
    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, GHOST_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - GHOST_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - GHOST_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, GHOST_SPEED + position.y, i_map);

    // Setting target based on current movement mode
    if(cur_movement_mode == MovementMode::Chase_mode)
    {
        target = i_pacman.getPosition();
    }
    else if(cur_movement_mode == MovementMode::Scatter_mode)
    {
        target = RED_GHOST_SCATTER_TARGET;
    }

    // Setting direction based on current movement mode
    if(cur_movement_mode == MovementMode::Frightened_mode)
    {
        set_random_direction(walls,direction,GHOST_SPEED);
    }
    else
    {
        set_optimal_direction(walls, direction , GHOST_SPEED ,position, target);
    }
    
    // If in frightened mode move ghost only after some amount of frames
    if(frightened_move_lag > 0 && cur_movement_mode == MovementMode::Frightened_mode)
    {
        frightened_move_lag --;
        move = 0;
    }
    else if(frightened_move_lag == 0 && cur_movement_mode == MovementMode::Frightened_mode)
    {
        frightened_move_lag = GHOST_FRIGHTENED_MOVE_LAG;
        move = 1;
    }
    else
    {
        move = 1;
    }

    if(!walls[direction] && move)
    {
        switch (direction)
        {
            case Direction::Right:
            {
                current_sprite_frame_edge = GHOST_RIGHT_FRAME_END;
                position.x += GHOST_SPEED;
                
                break;
            }
            case Direction::Up:
            {
                current_sprite_frame_edge = GHOST_UP_FRAME_END;
                position.y -= GHOST_SPEED;

                break;
            }
            case Direction::Left:
            {
                current_sprite_frame_edge = GHOST_LEFT_FRAME_END;
                position.x -= GHOST_SPEED;

                break;
            }
            case Direction::Down:
            {
                current_sprite_frame_edge = GHOST_DOWN_FRAME_END;
                position.y += GHOST_SPEED;
            }

        }
    }

    if (position.x <= -CELL_SIZE )
	{
        position.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
	}
	else if (position.x >= CELL_SIZE * MAP_WIDTH)
	{
        position.x = PACMAN_SPEED - CELL_SIZE;
	}
}

Position RedGhost::getPosition()
{
    return position;
};

void RedGhost::reset()
{
    position = home;
    direction = 0;
    target = RED_GHOST_SCATTER_TARGET;
}