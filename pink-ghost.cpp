#include <SFML/Graphics.hpp>
#include <iostream>

#include "headers/global.hpp"
#include "headers/ghost.hpp"
#include "headers/map-collision.hpp"

void Ghost::draw(sf::RenderWindow &i_window)
{
    // Circle of radius = CELL_SIZE / 2 = 8
    // sf::CircleShape circle(CELL_SIZE / 2);
    sf::CircleShape circle(CELL_SIZE / 2, 5);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(position.x,position.y);
    i_window.draw(circle);

    i_window.draw(circle);
}

void Ghost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void Ghost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
    unsigned char available_ways = 0;

    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, GHOST_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - GHOST_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - GHOST_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, GHOST_SPEED + position.y, i_map);

    for(unsigned char a = 0; a < 4; a++)
    {
        /*
         Ghosts should never go backwards based on their current direction 
         ie if direction == Right(0) : Left(2) should be inaccessible
            if direction == Left(2) : Right(0) should be inaccessible
            if direction == Up(1) : Down(3) should be inaccessible
            if direction == Down(3) : Up(1) should be inaccessible
            (direction + 2) % 4 refers to the inaccessible direction for all cases
        */
        unsigned char inaccessibleRoute = (direction + 2) % 4;

        if(a == inaccessibleRoute)
        {
            continue;
        }
        else if(!walls[a])
        {
            available_ways++;
        }
    }

    // Check for places where there is more than a single entry point 
    if(available_ways > 1)
    {
        std::cout<< "############ At junction #############" <<std::endl;
        unsigned char new_direction = rand() % 4;

        if(!walls[new_direction] && new_direction != (direction + 2) % 4)
        {
            direction = new_direction;
        }
    }
    // If there's still a wall in the current direction generate a a new one that isn't blocked by a wall
    else if(walls[direction])
    {
         for(unsigned char a = 0; a < 4; a++)
         {
             if(!walls[a] && a != (direction + 2) % 4)
             {
                direction = a;

                break;
             }
         }
    }

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