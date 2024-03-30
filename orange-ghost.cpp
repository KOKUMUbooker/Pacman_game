#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/map-collision.hpp"
#include "headers/orange-ghost.hpp"

OrangeGhost::OrangeGhost():use_door{1},direction{2} {}

void OrangeGhost::draw(sf::RenderWindow &i_window)
{
    sf::Texture texture;
    texture.loadFromFile("./assets/orange-ghost.png");
    //Setup our sprite with a texture
    sf::Sprite sprite(texture);
    sprite.setScale(0.55f,0.55f);
    sprite.setPosition(position.x,position.y);
    i_window.draw(sprite);

    // sf::CircleShape circle(CELL_SIZE / 2, 5);
    // circle.setFillColor(sf::Color(255, 165, 0));
    // circle.setPosition(position.x,position.y);
    // i_window.draw(circle);
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

void OrangeGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman)
{
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
         //The orange ghost will chase Pacman until it gets close to him. Then it'll switch to the scatter mode.
        if (CELL_SIZE * GHOST_3_CHASE <= sqrt(pow(position.x - i_pacman.getPosition().x, 2) + pow(position.y - i_pacman.getPosition().y, 2)))
		{
			target = i_pacman.getPosition();
		}
		else
		{
			target = {0, CELL_SIZE * (MAP_HEIGHT - 1)};
		}
    }

    set_optimal_direction(walls, direction, GHOST_SPEED ,position ,target);
    
    if(!walls[direction])
    {
        switch (direction)
        {
            case 0: // Right
            {
                position.x += GHOST_SPEED;
                
                break;
            }
            case 1:  // Up
            {
                position.y -= GHOST_SPEED;

                break;
            }
            case 2:  // Left
            {
                position.x -= GHOST_SPEED;

                break;
            }
            case 3:  // Down
            {
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