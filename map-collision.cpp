#include <array>
#include <cmath>
#include <iostream>

#include "headers/global.hpp"
#include "headers/map-collision.hpp"


bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
	bool output = 0;

	// Getting the exact position.
	float cell_x = i_x / static_cast<float>(CELL_SIZE);
	float cell_y = i_y / static_cast<float>(CELL_SIZE);

	if(floor(cell_x) >= 0 && floor(cell_y) >= 0 && MAP_HEIGHT > floor(cell_y) && MAP_WIDTH >floor(cell_x))
	{
		if(i_map[floor(cell_x)][floor(cell_y)] == Cell::Wall)
		{
			output = 1;
		}
		else if(i_collect_pellets && i_map[floor(cell_x)][floor(cell_y)] == Cell::Pellet)
		{
			i_map[floor(cell_x)][floor(cell_y)] = Cell::Empty;
		}
	}

	if(ceil(cell_x) >= 0 && floor(cell_y) >= 0 && MAP_HEIGHT > floor(cell_y) && MAP_WIDTH >ceil(cell_x))
	{
		if(i_map[ceil(cell_x)][floor(cell_y)] == Cell::Wall)
		{
			output = 1;
		}
		else if(i_collect_pellets && i_map[ceil(cell_x)][floor(cell_y)] == Cell::Pellet)
		{
			i_map[ceil(cell_x)][floor(cell_y)] = Cell::Empty;
		}
	}

	if(floor(cell_x) >= 0 && ceil(cell_y) >= 0 && MAP_HEIGHT > ceil(cell_y) && MAP_WIDTH >floor(cell_x))
	{
		if(i_map[floor(cell_x)][ceil(cell_y)] == Cell::Wall)
		{
			output = 1;
		}
		else if(i_collect_pellets && i_map[floor(cell_x)][ceil(cell_y)] == Cell::Pellet)
		{
			i_map[floor(cell_x)][ceil(cell_y)] = Cell::Empty;
		}
	}

	if(ceil(cell_x) >= 0 && ceil(cell_y) >= 0 && MAP_HEIGHT > ceil(cell_y) && MAP_WIDTH >ceil(cell_x))
	{
		if(i_map[ceil(cell_x)][ceil(cell_y)] == Cell::Wall)
		{
			output = 1;
		}
		else if(i_collect_pellets && i_map[ceil(cell_x)][ceil(cell_y)] == Cell::Pellet)
		{
			i_map[ceil(cell_x)][ceil(cell_y)] = Cell::Empty;
		}
	}
	

	return output;
}