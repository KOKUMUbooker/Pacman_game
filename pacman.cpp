#include <SFML/Graphics.hpp>
#include <iostream>

#include "headers/global.hpp"
#include "headers/pacman.hpp"
#include "headers/map-collision.hpp"

void Pacman::draw(sf::RenderWindow &i_window)
{
    // Circle of radius = CELL_SIZE / 2 = 8
    sf::CircleShape circle(CELL_SIZE / 2);
    circle.setFillColor(sf::Color(255,255,0));
    circle.setPosition(position.x,position.y);

    i_window.draw(circle);
}

void Pacman::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void Pacman::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, PACMAN_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - PACMAN_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - PACMAN_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, PACMAN_SPEED + position.y, i_map);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {   
        if(!walls[3])
        {
            direction = 3;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        if(!walls[2])
        {
            direction = 2;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if(!walls[0])
        {
            direction = 0;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        if(!walls[1])
        {
            direction = 1;
        }
    }

    // switch (direction)
    // {
    //     case 0:
    //     {
    //         if(map_collision(1, 0, PACMAN_SPEED + position.x,position.y,i_map) == 0)
    //         {
    //             position.x += PACMAN_SPEED;
    //         }
    //         break;
    //     }
    //     case 1:
    //     {
    //         if(!map_collision(1, 0, position.x, position.y - PACMAN_SPEED,i_map))
    //         {
    //             position.y -= PACMAN_SPEED;
    //         }
    //         break;
    //     }
    //     case 2:
    //     {
    //         if(!map_collision(1, 0, position.x - PACMAN_SPEED, position.y, i_map))
    //         {
    //             position.x -= PACMAN_SPEED;
    //         }
    //         break;
    //     }
    //     case 3:
    //     {
    //         if(!map_collision(1, 0, position.x,PACMAN_SPEED + position.y,i_map))
    //         {
    //             position.y += PACMAN_SPEED;
    //         }
    //         break;
    //     }
    
    // }
    if (!walls[direction])
	{
		switch (direction)
		{
			case 0:
			{
				position.x += PACMAN_SPEED;

				break;
			}
			case 1:
			{
				position.y -= PACMAN_SPEED;

				break;
			}
			case 2:
			{
				position.x -= PACMAN_SPEED;

				break;
			}
			case 3:
			{
				position.y += PACMAN_SPEED;
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

    map_collision(1, 0, position.x, position.y, i_map);
}

Position Pacman::getPosition(){
    return position;
};