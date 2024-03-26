#include <map>
#include <cmath>
#include <iostream>

#include "headers/global.hpp"
#include "headers/set-optimal-direction.hpp"

short getDistApart(Position target,AvailablePositions current){
    short x = current.x - target.x;
    short y = current.y - target.y;

    return std::sqrt((std::pow(x,2)+std::pow(y,2)));
}

void set_optimal_direction(std::array<bool, 4> &walls, unsigned char &user_direction ,Position user_position, Position target_position)
{
    std::map<unsigned char,AvailablePositions> available_paths {};
    unsigned char direction {0};
    
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
        unsigned char inaccessibleRoute = (user_direction + 2) % 4;

        if(a == inaccessibleRoute)
        {
            continue;
        }
        else if(!walls[a])
        {
            // Checks to find out how far from the cell in that direction is the target and store this info
            switch (a)
            {
                case 0:
                {
                    AvailablePositions position0{static_cast<short>(GHOST_SPEED + user_position.x),user_position.y};
                    position0.targetDist = getDistApart(target_position,position0);
                    available_paths.insert({0,position0});
                    break;
                }
                case 1:
                {
                    AvailablePositions position1{user_position.x, static_cast<short>(user_position.y - GHOST_SPEED)};
                    position1.targetDist = getDistApart(target_position,position1);
                    available_paths.insert({1,position1});
                    break;
                }
                case 2:
                {
                    AvailablePositions position2{static_cast<short>(user_position.x - GHOST_SPEED), user_position.y};
                    position2.targetDist = getDistApart(target_position,position2);
                    available_paths.insert({2,position2});
                    break;
                }
                case 3:
                {
                    AvailablePositions position3{user_position.x, static_cast<short>(GHOST_SPEED + user_position.y)};
                    position3.targetDist = getDistApart(target_position,position3);
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
            if (pair.second.targetDist < minTargetDist) {             
                minTargetDist = pair.second.targetDist;
                minKey = pair.first;
            }
        }

            if(!walls[minKey] )
            {
                user_direction = minKey;
            }
    }
    // If there's still a wall in the current direction check all directions to find one without a wall
    else if(walls[user_direction])
    {
         for(unsigned char a = 0; a < 4; a++)
         {
             if(!walls[a] && a != (user_direction + 2) % 4)
             {
                 user_direction = a;

                break;
             }
         }
    }

    available_paths.clear();    
}