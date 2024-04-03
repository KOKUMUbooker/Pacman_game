#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/red-ghost.hpp"
#include "headers/map-collision.hpp"

RedGhost::RedGhost():direction{0},use_door{0},current_sprite_frame_edge{0},frightened_move_lag{GHOST_FRIGHTENED_MOVE_LAG}{}

void RedGhost::draw(sf::RenderWindow &i_window, sf::Clock &animation_clock, const MovementMode &cur_movement_mode)
{
    sf::Texture texture;
    texture.loadFromFile("./assets/sprite_sheets/red_ghost.png"); 
    if(cur_movement_mode == MovementMode::Frightened_mode)  current_sprite_frame_edge = GHOST_FRIGHTENED_FRAME_END ;

    sf::IntRect rectSourceSprite(current_sprite_frame_edge,0,24,24);  // width = 24 , height = 24  
    sf::Sprite sprite(texture,rectSourceSprite);
    sprite.setScale(0.65f,0.65f);
    sprite.setPosition(position.x,position.y);
    ghost_sprite = sprite;

    // After a specified duration we change the sprite section currently in view
 if (animation_clock.getElapsedTime().asSeconds() > GHOST_FRAME_SWITCH_DURATION && cur_movement_mode != MovementMode::Frightened_mode)
    {
        if(rectSourceSprite.left == current_sprite_frame_edge)
        {
            rectSourceSprite.left =  current_sprite_frame_edge - SPRITE_GAME_CHARACTER_WIDTH;
        }
        else{
            rectSourceSprite.left = current_sprite_frame_edge;
        }

        sprite.setTextureRect(rectSourceSprite);
        animation_clock.restart();
    }

    i_window.draw(sprite);
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
            i_pacman.set_dead(1);
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
            case 0:
            {
                current_sprite_frame_edge = GHOST_RIGHT_FRAME_END;
                position.x += GHOST_SPEED;
                
                break;
            }
            case 1:
            {
                current_sprite_frame_edge = GHOST_UP_FRAME_END;
                position.y -= GHOST_SPEED;

                break;
            }
            case 2:
            {
                current_sprite_frame_edge = GHOST_LEFT_FRAME_END;
                position.x -= GHOST_SPEED;

                break;
            }
            case 3:
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
