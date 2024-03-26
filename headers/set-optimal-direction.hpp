#pragma once

#include <array>
#include <map>
#include "global.hpp"
#include <string>

short getDistApart(Position target,AvailablePositions current);

// walls = Infomation about the walls / inaccessible routes around an item 
//         Thus collision checking on all directions MUST be done before using this function
// user_position = current position of the entity
// target_position = current position of the target
void set_optimal_direction(std::array<bool, 4> &walls, unsigned char &user_direction ,Position user_position, Position target_position);

Position getMapCoordinatesInGrid(Position Position);

std::string getStringDirection(unsigned char direction);

std::vector<Position> getOptimalPathCoordinates(Position user_position, Position target_position,unsigned char initial_user_dir ,std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> i_map);
