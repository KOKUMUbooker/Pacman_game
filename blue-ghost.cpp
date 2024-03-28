#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <iostream>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/map-collision.hpp"
#include "headers/blue-ghost.hpp"
#include "headers/red-ghost.hpp"

BlueGhost::BlueGhost():use_door{1},direction{1} {}

void BlueGhost::draw(sf::RenderWindow &i_window)
{
    sf::CircleShape circle(CELL_SIZE / 2, 5);
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(position.x,position.y);
    i_window.draw(circle);
}

void BlueGhost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void BlueGhost::set_target(short i_x,short i_y)
{
    target = {i_x,i_y};
}

void BlueGhost::set_home_exit(short i_x,short i_y)
{
    home_exit = {i_x,i_y};
}

void BlueGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman,Position i_red_ghost_position)
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
        // 1) First the Blue ghost targets 2 tiles ahead of pacman
        Position new_target {i_pacman.getPosition().x, i_pacman.getPosition().y};
        
        switch (i_pacman.getDirection())
        {
            case 0: // Right
            {
                new_target.x += CELL_SIZE * 2;
                break;
            }
            case 1: // Up
            {
                new_target.y += CELL_SIZE * 2;
                break;
            }
            case 2:  // Left
            {
                new_target.x -= CELL_SIZE * 2;
                break;
            }
            case 3:  // Down
            {
                new_target.y -= CELL_SIZE * 2;
                break;
            }
        }

        Position two_tiles_ahead_pacman = new_target;

        // 2) Create a vector to this new_target from the red_ghost and double it
        short doubleDistanceApart = getDistApart(two_tiles_ahead_pacman,AvailablePositions {i_red_ghost_position.x,i_red_ghost_position.y}) * 2;

        // 3) Retrieve coordinate of where it lands which will be the new target 
               // i) Get angle between vector and perpendicular height
               double angle = get_angle_between_perpendicular_height_and_vector(i_red_ghost_position,two_tiles_ahead_pacman);

               // ii) Using this angle we can use SOH CAH TOA to get location of endpoint of vector
               target = get_unknown_coordinate(i_red_ghost_position, doubleDistanceApart, angle);
               set_target(target.x,target.y);
               
            //    std::cout<< "Target for Blue 🔵🔵  x :"<<target.x<<", y : "<<target.y<<std::endl;
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