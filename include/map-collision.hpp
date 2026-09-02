#pragma once

#include <array>
#include "global.hpp"

bool map_collision(bool i_collect_pellets, bool i_use_door ,short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map);

bool sprite_collision(sf::Sprite sprite1, sf::Sprite sprite2);
