#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <map>

#include "headers/global.hpp"
#include "headers/red-ghost.hpp"
#include "headers/map-collision.hpp"

short getDistApart(Position target,AvailablePositions current){
    short x = current.x - target.x;
    short y = current.y - target.y;

    return std::sqrt((std::pow(x,2)+std::pow(y,2)));
}

void RedGhost::draw(sf::RenderWindow &i_window)
{
    // Circle of radius = CELL_SIZE / 2 = 8
    // sf::CircleShape circle(CELL_SIZE / 2);
    sf::CircleShape circle(CELL_SIZE / 2, 5);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(position.x,position.y);
    i_window.draw(circle);

    i_window.draw(circle);
}

void RedGhost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void RedGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman)
{
    std::map<unsigned char,AvailablePositions> available_paths {};

    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, GHOST_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - GHOST_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - GHOST_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, GHOST_SPEED + position.y, i_map);
    // std::cout<<"************ for walls[3]  x :"<<std::to_string(position.x)<<" y : "<<std::to_string(GHOST_SPEED + position.y)<<" *****************"<<std::endl;

    // Loop to get number of all available paths based on current location
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
            // Check to store the positions of available paths based on the direction
            switch (a)
            {
                case 0:
                {
                    AvailablePositions position0{static_cast<short>(GHOST_SPEED + position.x),position.y};
                    position0.targetDist = getDistApart(i_pacman.getPosition(),position0);
                    available_paths.insert({0,position0});
                    break;
                }
                case 1:
                {
                    AvailablePositions position1{position.x, static_cast<short>(position.y - GHOST_SPEED)};
                    position1.targetDist = getDistApart(i_pacman.getPosition(),position1);
                    available_paths.insert({1,position1});
                    break;
                }
                case 2:
                {
                    AvailablePositions position2{static_cast<short>(position.x - GHOST_SPEED), position.y};
                    position2.targetDist = getDistApart(i_pacman.getPosition(),position2);
                    available_paths.insert({2,position2});
                    break;
                }
                case 3:
                {
                    AvailablePositions position3{position.x, static_cast<short>(GHOST_SPEED + position.y)};
                    position3.targetDist = getDistApart(i_pacman.getPosition(),position3);
                    available_paths.insert({3,position3});
                    break;
                }

            }
        }
    }

    // Check for places where there is more than a single entry point 
    if(available_paths.size() > 1)
    {
    // Useful for determining the least target distance from pacman 
    short minTargetDist = std::numeric_limits<short>::max();
    unsigned char minKey = 0; // Variable to store the key corresponding to the minimum targetDist

     for (const auto& pair : available_paths) {
        std::cout << "Key: " << static_cast<int>(pair.first) << std::endl; // Print the key
        std::cout << "x: " << pair.second.x << ", y: " << pair.second.y << std::endl; // Print x and y values
        std::cout << "Pacman x: " <<i_pacman.getPosition().x << ",Pacman y: " << i_pacman.getPosition().y << std::endl; // Print x and y values
        std::cout << "Target Distance: " << pair.second.targetDist << std::endl; // Print target distance if needed
        std::cout << std::endl; // Add a newline for clarity
         if (pair.second.targetDist < minTargetDist) {
            minTargetDist = pair.second.targetDist;
            minKey = pair.first;
        }
        
      }

        std::cout<< "############ At junction with shortest path being : "<< std::to_string(minKey) << " with dist of "<< std::to_string(minTargetDist)<<" #############" <<std::endl;

        if(!walls[minKey] )
        {
            direction = minKey;
        }
    }
    // If there's still a wall in the current direction generate a a new one that isn't blocked by a wall
    else if(walls[direction])
    {
         for(unsigned char a = 0; a < 4; a++)
         {
             if(!walls[a] && a != (direction + 2) % 4)
             {
                std::cout<< "😖😖😖😖😖😖😖😖😖😖😖 Wall found : now switching to : "<<std::to_string(a)<<" 😖😖😖😖😖😖😖😖😖" <<std::endl;
                direction = a;

                break;
             }
         }
    }

    if(!walls[direction])
    {
        available_paths.clear();
         std::cout << "======================  ********************* ===============================" << std::endl;
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