#pragma once
#include <SFML/Graphics.hpp>
#include "global.hpp"

class Ghost
{
    Position position;
    unsigned char direction; // 0 = Right, 1 = Up, 2 = left, 3 = Down

    public:
        void draw(sf::RenderWindow &i_window);
        void set_position(short i_x ,short i_y);
        void update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map);
};