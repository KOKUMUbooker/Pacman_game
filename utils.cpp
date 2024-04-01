#include <map>
#include <cmath>
#include <iostream>
#include <vector>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/map-collision.hpp"

short getDistApart(Position target,AvailablePositions current){
    short x = current.x - target.x;
    short y = current.y - target.y;

    return std::sqrt((std::pow(x,2)+std::pow(y,2)));
}

std::string getStringDirection(unsigned char direction)
{
    // 0 = Right, 1 = Up, 2 = left, 3 = Down
    if(direction == 0) { return "Right";}
    else if(direction == 1) { return "Up";}
    else if(direction == 2) { return "Left";}
    else if(direction == 3) { return "Down";}
    else return "";
}

Position getMapCoordinatesInGrid(Position position){
    short cell_x = ceil(position.x / CELL_SIZE);
    short cell_y = ceil(position.y / CELL_SIZE);

    return Position {static_cast<short>(cell_x * 16), static_cast<short>(cell_y *16)};
}

double convertDegreesToRadians(double degrees) {
  return degrees * M_PI / 180.0;
}

// NOTE: Returns angle in radians
double get_angle_between_perpendicular_height_and_vector(Position &vector_start, Position &vector_end)
{
    short base {static_cast<short>(vector_end.x - vector_start.x)};
    short height {static_cast<short>(vector_end.y - vector_start.y)};
    double tan_value = static_cast<double>(base) / static_cast<double>(height);
    
    // Calculate the angle in radians
    double angle_in_radians = atan(tan_value);

    return angle_in_radians;
}

Position get_unknown_coordinate(const Position &known_position, double distance ,double angle_in_radians)
{
    // _________(a,b) distance_end_point
    // ||     //
    // ||    // 
    // ||   //
    // ||  //
    // || //
    // ||//
    // (x,y) known_position

    // 1) For the triangle formed by the distance end point and the known point : 
    //    i)Get their base and height
        double a = distance * sin(angle_in_radians);  // along x , thus is the base 
        double b = distance * cos(angle_in_radians);  // along y , thus is the height

    //    ii) Using the base and height we can now get coordinates of the distance end point
        Position distance_end_point {};
        distance_end_point.x = static_cast<short>(a) + known_position.x;
        distance_end_point.y = static_cast<short>(b) + known_position.y;

        return distance_end_point;
}

void set_optimal_direction(std::array<bool, 4> &walls, unsigned char &user_direction ,unsigned char distance_offset ,Position user_position, Position target_position )
{
    std::map<unsigned char,AvailablePositions> available_paths {};
    // std::cout<<"Initial user_direction in set_optimal_direction fn : "<<std::to_string(user_direction)<<std::endl;
    
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
                    AvailablePositions position0{static_cast<short>(distance_offset + user_position.x),user_position.y};
                    position0.targetDist = getDistApart(target_position,position0);
                    available_paths.insert({0,position0});
                    break;
                }
                case 1:
                {
                    AvailablePositions position1{user_position.x, static_cast<short>(user_position.y - distance_offset)};
                    position1.targetDist = getDistApart(target_position,position1);
                    available_paths.insert({1,position1});
                    break;
                }
                case 2:
                {
                    AvailablePositions position2{static_cast<short>(user_position.x - distance_offset), user_position.y};
                    position2.targetDist = getDistApart(target_position,position2);
                    available_paths.insert({2,position2});
                    break;
                }
                case 3:
                {
                    AvailablePositions position3{user_position.x, static_cast<short>(distance_offset + user_position.y)};
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
    // std::cout<<"Final user_direction in set_optimal_direction fn : "<<std::to_string(user_direction)<<std::endl;
    
    available_paths.clear();    
}
