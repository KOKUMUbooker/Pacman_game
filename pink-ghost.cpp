#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "headers/global.hpp"
#include "headers/set-optimal-direction.hpp"
#include "headers/map-collision.hpp"
#include "headers/pink-ghost.hpp"

PinkGhost::PinkGhost():use_door{1} {}

void PinkGhost::draw(sf::RenderWindow &i_window)
{
    sf::CircleShape circle(CELL_SIZE / 2, 5);
    circle.setFillColor(sf::Color(252, 3, 173));
    circle.setPosition(position.x,position.y);
    i_window.draw(circle);
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

void PinkGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman)
{
    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, use_door, GHOST_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, use_door, position.x, position.y - GHOST_SPEED, i_map);
	walls[2] = map_collision(0, use_door, position.x - GHOST_SPEED, position.y, i_map);
	walls[3] = map_collision(0, use_door, position.x, GHOST_SPEED + position.y, i_map);
    
    target = use_door ? home_exit : i_pacman.getPosition() ;
    if(position == home_exit)
    {
        use_door = 0; // Prevent re-entry to home
        is_out_of_house = 1;
        target = i_pacman.getPosition(); // Set new target
    }
    else if(!use_door)
    {
        // The pink ghost targets 4 tiles ahead of pacman
        Position pos = getMapCoordinatesInGrid(position);
        std::cout<< "\n========================================= "<<std::endl;
        std::cout<< "Actual coordinates x : "<<pos.x<<" y : "<<pos.y<<std::endl;
        std::vector<Position> coordinates = getOptimalPathCoordinates(position,i_pacman.getPosition(),direction,i_map); 
        std::cout<< "No of cells to get to target "<<std::to_string(coordinates.size())<<" 😎😎😎😎😎 "<<std::endl;
        std::cout<< " ========================================= \n"<<std::endl;   

    }
    set_optimal_direction(walls, direction,position ,target);

    if(!walls[direction])
    {
        switch (direction)
        {
            case 0:
            {
                position.x += GHOST_SPEED;
                
                break;
            }
            case 1:
            {
                position.y -= GHOST_SPEED;

                break;
            }
            case 2:
            {
                position.x -= GHOST_SPEED;

                break;
            }
            case 3:
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