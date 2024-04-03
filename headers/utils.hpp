#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>
#include "global.hpp"

short get_euclidean_distance(Position target,AvailablePositions current);

short get_manhattan_distance(Position user_position,Position target_position);

// walls = Infomation about the walls / inaccessible routes around an item 
//         Thus collision checking on all directions MUST be done before using this function
// user_position = current position of the entity
// target_position = current position of the target
void set_optimal_direction(std::array<bool, 4> &walls, unsigned char &user_direction, unsigned char distance_offset ,Position user_position, Position target_position);

Position get_map_coordinates_in_grid(Position Position);

std::string get_string_direction(unsigned char direction);

std::string get_ghost_mode(MovementMode ghost_mode);

double get_angle_between_perpendicular_height_and_vector( Position &vector_start, Position &vector_end);

Position get_unknown_coordinate(const Position &known_position, double distance ,double angle_in_radians);

void set_random_direction(std::array<bool, 4> &walls, unsigned char &user_direction ,unsigned char distance_offset );
