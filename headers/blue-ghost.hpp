#pragma once
#include <SFML/Graphics.hpp>
#include "global.hpp"
#include "pacman.hpp"

class BlueGhost
{
    Position position;
    unsigned char direction; // 0 = Right, 1 = Up, 2 = left, 3 = Down
    bool use_door;
    Position target;
	Position home_exit;
    Position home;
    short current_sprite_frame_edge = 0;

    public:
        BlueGhost();
        void draw(sf::RenderWindow &i_window,sf::Clock &animation_clock);
        void set_position(short i_x ,short i_y);
        void set_target(short i_x ,short i_y);
        void set_home_exit(short i_x ,short i_y);
        void set_home(short i_x ,short i_y);
        void reset();
        void update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman,Position i_red_ghost_position,MovementMode &cur_movement_mode);
};