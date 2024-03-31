#pragma once
#include <SFML/Graphics.hpp>
#include "global.hpp"
#include "pacman.hpp"

class PinkGhost
{
    Position position;
    unsigned char direction; // 0 = Right, 1 = Up, 2 = left, 3 = Down
    bool use_door;
    bool is_out_of_house;
    Position target;
	Position home_exit;
    short current_sprite_frame_edge = 0;

    public:
        PinkGhost();
        void draw(sf::RenderWindow &i_window,sf::Clock &animation_clock);
        void set_position(short i_x ,short i_y);
        void set_target(short i_x ,short i_y);
        void set_home_exit(short i_x ,short i_y);
        void update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman);
};