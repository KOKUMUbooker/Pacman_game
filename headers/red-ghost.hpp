#pragma once
#include <SFML/Graphics.hpp>
#include "global.hpp"
#include "pacman.hpp"

class RedGhost
{
    Position position;
    Position home;
    Position target;
    unsigned char direction; // 0 = Right, 1 = Up, 2 = left, 3 = Down
    bool use_door;
    short current_sprite_frame_edge;
    sf::Sprite ghost_sprite;

    public:
        RedGhost();
        Position getPosition();
        void draw(sf::RenderWindow &i_window,sf::Clock &animation_clock, const MovementMode &cur_movement_mode);
        void set_position(short i_x ,short i_y);
        void update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman, MovementMode &cur_movement_mode);
        void set_home(short i_x ,short i_y);
        void reset();
};