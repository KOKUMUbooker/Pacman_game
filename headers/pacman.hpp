#pragma once
#include <SFML/Graphics.hpp>
#include "global.hpp"

class Pacman
{
    Position position;
    unsigned char direction; // 0 = Right, 1 = Up, 2 = left, 3 = Down
    short current_sprite_frame_top_distance = 0;
    short current_sprite_frame_edge_x_axis = 0;

    public:
        void draw(sf::RenderWindow &i_window, sf::Clock &animation_clock);
        void set_position(short i_x ,short i_y);
        void update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, MovementMode &cur_movement_mode);
        Position getPosition();
        unsigned char getDirection();
};