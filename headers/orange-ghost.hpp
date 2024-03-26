#pragma once
#include <SFML/Graphics.hpp>
#include "global.hpp"
#include "pacman.hpp"

class OrangeGhost
{
    Position position;
    unsigned char direction; // 0 = Right, 1 = Up, 2 = left, 3 = Down
    bool use_door;
    Position target;
	Position home_exit;

    public:
        OrangeGhost();
        void draw(sf::RenderWindow &i_window);
        void set_position(short i_x ,short i_y);
        void set_target(short i_x ,short i_y);
        void set_home_exit(short i_x ,short i_y);
        void update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman);
};