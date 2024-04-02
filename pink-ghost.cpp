#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/map-collision.hpp"
#include "headers/pink-ghost.hpp"

PinkGhost::PinkGhost():use_door{1},direction{0} {}

void PinkGhost::draw(sf::RenderWindow &i_window,sf::Clock &animation_clock)
{
    sf::Texture texture;
    texture.loadFromFile("./assets/sprite_sheets/pink_ghost.png");

    sf::IntRect rectSourceSprite(current_sprite_frame_edge,0,24,24);  // width = 24 , height = 24  
    sf::Sprite sprite(texture,rectSourceSprite);
    sprite.setScale(0.65f,0.65f);
    sprite.setPosition(position.x,position.y);

    // After a specified duration we change the sprite section currently in view
    if(animation_clock.getElapsedTime().asSeconds() > GHOST_FRAME_SWITCH_DURATION)
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

void PinkGhost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void PinkGhost::set_target(short i_x,short i_y)
{
    target = {i_x,i_y};
}

void PinkGhost::set_home_exit(short i_x,short i_y)
{
    home_exit = {i_x,i_y};
}

void PinkGhost::set_home(short i_x,short i_y)
{
    home = {i_x,i_y};
}


void PinkGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman, MovementMode &cur_movement_mode)
{
    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, use_door, GHOST_SPEED + position.x, position.y, i_map,cur_movement_mode);
	walls[1] = map_collision(0, use_door, position.x, position.y - GHOST_SPEED, i_map,cur_movement_mode);
	walls[2] = map_collision(0, use_door, position.x - GHOST_SPEED, position.y, i_map,cur_movement_mode);
	walls[3] = map_collision(0, use_door, position.x, GHOST_SPEED + position.y, i_map,cur_movement_mode);
    
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
            target = PINK_GHOST_SCATTER_TARGET;
        }
        else if(cur_movement_mode == MovementMode::Chase_mode)
        {
            // The pink ghost targets 4 tiles ahead of pacman
            Position new_target {i_pacman.getPosition().x,i_pacman.getPosition().y};
            
            switch (i_pacman.getDirection())
            {
                case 0: // Right
                {
                    new_target.x += CELL_SIZE * 4;
                    break;
                }
                case 1: // Up
                {
                    new_target.y += CELL_SIZE * 4;
                    break;
                }
                case 2:  // Left
                {
                    new_target.x -= CELL_SIZE * 4;
                    break;
                }
                case 3:  // Down
                {
                    new_target.y -= CELL_SIZE * 4;
                    break;
                }
                
            }

            set_target(new_target.x,new_target.y);
            // std::cout<< "\n==================================================="<<std::endl;
            // std::cout<< "Target for Pink   x :"<<target.x<<", y : "<<target.y<<std::endl;
            // std::cout<< "Target for Red   x :"<<i_pacman.getPosition().x<<", y : "<<i_pacman.getPosition().y<<std::endl;        
            // std::cout<< "===================================================\n"<<std::endl;
            }
    }

    set_optimal_direction(walls, direction, GHOST_SPEED,position ,target);
    
    if(!walls[direction])
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

void PinkGhost::reset()
{
    use_door = 1;
    direction = 0;
    position = home;
    target = home_exit;
}