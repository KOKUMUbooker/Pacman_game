#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/map-collision.hpp"
#include "headers/orange-ghost.hpp"

OrangeGhost::OrangeGhost():use_door{1},direction{2},frightened_move_lag{GHOST_FRIGHTENED_MOVE_LAG} {}

void OrangeGhost::draw(sf::RenderWindow &i_window,sf::Clock &animation_clock, const MovementMode &cur_movement_mode)
{
    sf::Texture texture;
    texture.loadFromFile("./assets/sprite_sheets/orange_ghost.png");
    if(cur_movement_mode == MovementMode::Frightened_mode)  current_sprite_frame_edge = GHOST_FRIGHTENED_FRAME_END ;

    sf::IntRect rectSourceSprite(current_sprite_frame_edge,0,24,24);  // width = 24 , height = 24  
    sf::Sprite sprite(texture,rectSourceSprite);
    sprite.setScale(0.65f,0.65f);
    sprite.setPosition(position.x,position.y);
    ghost_sprite = sprite;

    // After a specified duration we change the sprite section currently in view
    if(animation_clock.getElapsedTime().asSeconds() > GHOST_FRAME_SWITCH_DURATION && cur_movement_mode != MovementMode::Frightened_mode)
    {
        if(rectSourceSprite.left == current_sprite_frame_edge)
        {
            rectSourceSprite.left =  current_sprite_frame_edge - SPRITE_GAME_CHARACTER_WIDTH;
        }
        else{
            rectSourceSprite.left = current_sprite_frame_edge ;
        }

        sprite.setTextureRect(rectSourceSprite);
        animation_clock.restart();
    }
    i_window.draw(sprite);
}

void OrangeGhost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void OrangeGhost::set_target(short i_x,short i_y)
{
    target = {i_x,i_y};
}

void OrangeGhost::set_home_exit(short i_x,short i_y)
{
    home_exit = {i_x,i_y};
}

void OrangeGhost::set_home(short i_x,short i_y)
{
    home = {i_x,i_y};
}

void OrangeGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman, MovementMode &cur_movement_mode)
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

    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, use_door, GHOST_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, use_door, position.x, position.y - GHOST_SPEED, i_map);
	walls[2] = map_collision(0, use_door, position.x - GHOST_SPEED, position.y, i_map);
	walls[3] = map_collision(0, use_door, position.x, GHOST_SPEED + position.y, i_map);
    
    if(position == home_exit && use_door == 1)
    {
        use_door = 0; // Prevent re-entry to home
        set_target(i_pacman.getPosition().x,i_pacman.getPosition().y);// Set new target;
    }
    else if(!(position == home_exit) && use_door == 1)
    {
        set_target(home_exit.x,home_exit.y);
    }
    else if(!use_door)
    {
        if(cur_movement_mode == MovementMode::Scatter_mode)
        {
            target = ORANGE_GHOST_SCATTER_TARGET;
        }
        else if(cur_movement_mode == MovementMode::Chase_mode)
        {
            short distance_to_target = get_manhattan_distance(position,i_pacman.getPosition());
            //The orange ghost will chase Pacman until it gets near pacman by 8 cells. Then it'll switch to the scatter mode.
            if (distance_to_target > CELL_SIZE * 8)
            {
                target = i_pacman.getPosition();
            }
            else
            {
                target = ORANGE_GHOST_SCATTER_TARGET;
            }
        }
    }

    // Setting direction based on current movement mode
    if(cur_movement_mode == MovementMode::Frightened_mode)
    {
        // Allows ghost to get out of home if was sent back by pacman
        Position pink_home = {static_cast<short>(home.x - CELL_SIZE), home.y};
        if(position == home || position == pink_home)
        {
            set_optimal_direction(walls, direction, GHOST_SPEED,position ,home_exit);
        }
        else
        {
            set_random_direction(walls,direction,GHOST_SPEED);
        }
    }
    else
    {
        set_optimal_direction(walls, direction, GHOST_SPEED ,position ,target);
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

void OrangeGhost::reset()
{
    use_door = 1;
    direction = 2;
    position = home;
    target = home_exit;
}
