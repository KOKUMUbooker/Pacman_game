#include <array>
#include <string>
#include <iostream>
#include "headers/global.hpp"
#include "headers/convert-sketch.hpp"
#include "headers/pacman.hpp"

std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(const std::array<std::string, MAP_HEIGHT>& i_map_sketch,Pacman& i_pacman){
    std::array<std::array<Cell,MAP_HEIGHT>,MAP_WIDTH> output_map {};

    for(unsigned char a = 0; a < MAP_HEIGHT; a++)
    {
        // Iterating through each of the 21 chars within the string
        for(unsigned char b = 0; b < MAP_WIDTH; b++)
        {
            switch (i_map_sketch[a][b])
            {
                case ' ':
                {
                    output_map[b][a] = Cell::Empty;

                    break;
                }
                case '#':
                {
                    output_map[b][a] = Cell::Wall;

                    break;
                }
                case '.':
                {
                    output_map[b][a] = Cell::Pellet;
                    // std::cout<<"Pellet found at ("<<std::to_string(b)<<","<<std::to_string(a)<<")"<<std::endl;

                    break;
                }
                case 'P':
                {
                    // b = how far along the x-axis, a = how far along the y-axis
                    i_pacman.set_position(CELL_SIZE * b,CELL_SIZE * a);

                    break;
                }
            }
        }
    }

    return output_map;
}