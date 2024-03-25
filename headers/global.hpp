#pragma once

constexpr unsigned char CELL_SIZE = 10;
constexpr unsigned char MAP_HEIGHT = 21;
constexpr unsigned char MAP_WIDTH = 21;
constexpr unsigned char SCREEN_RESIZE = 2;
constexpr unsigned char FONT_HEIGHT = 16;
constexpr unsigned char PACMAN_SPEED = 2;

constexpr unsigned short FRAME_DURATION = 16667;

enum Cell
{
	Door,
	Empty,
	Energizer,
	Pellet,
	Wall
};

struct Position
{
	short x;
	short y;

	// Overload the == operator to allow for equality comparison.
	bool operator==(const Position& i_position)
	{
		return this->x == i_position.x && this->y == i_position.y;
	}
};