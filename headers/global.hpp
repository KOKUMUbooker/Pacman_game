#pragma once

constexpr unsigned char CELL_SIZE = 16;
constexpr unsigned char MAP_HEIGHT = 21;
constexpr unsigned char MAP_WIDTH = 21;
constexpr unsigned char SCREEN_RESIZE = 2;
constexpr unsigned char FONT_HEIGHT = 16;
constexpr unsigned char PACMAN_SPEED = 2;
constexpr unsigned char GHOST_SPEED = 1;

constexpr unsigned short FRAME_DURATION = 16667;

enum Cell
{
	Door=1,
	Empty,
	Energizer,
	Pellet,
	Wall
};

struct AvailablePositions
{
	short x;
	short y;
	short targetDist = 0;
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