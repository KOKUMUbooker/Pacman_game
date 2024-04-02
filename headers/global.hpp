#pragma once

constexpr unsigned char CELL_SIZE {16};
constexpr unsigned char MAP_HEIGHT {21};
constexpr unsigned char MAP_WIDTH {21};
constexpr unsigned char SCREEN_RESIZE {2};
constexpr unsigned char FONT_HEIGHT {16};
constexpr unsigned char PACMAN_SPEED {2};
constexpr unsigned char GHOST_SPEED {1};
//If the distance between the orange ghost and Pacman is less than this value, the orange ghost will go into the scatter mode.
constexpr unsigned char GHOST_3_CHASE {4};
constexpr short SPRITE_GAME_CHARACTER_WIDTH {24}; // Size of each frame in each of the ghost sprite sheets 
constexpr short PACMAN_SPRITE_GAME_CHARACTER_WIDTH {16}; // Size of each frame for the pacman sheets 

constexpr float GHOST_FRAME_SWITCH_DURATION {0.3f};

// Frame limits for ghost sprites based on direction
constexpr short GHOST_UP_FRAME_END {168};
constexpr short GHOST_DOWN_FRAME_END {72};
constexpr short GHOST_LEFT_FRAME_END {120};
constexpr short GHOST_RIGHT_FRAME_END {24};

// Frame height level limits for pacman sprite based on direction
constexpr short PACMAN_UP_FRAME_END {16};
constexpr short PACMAN_DOWN_FRAME_END {48};
constexpr short PACMAN_LEFT_FRAME_END {32};
constexpr short PACMAN_RIGHT_FRAME_END {0};

constexpr unsigned short PACMAN_ENERGIZED_DURATION {512};

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

enum MovementMode
{
	Scatter_mode,
	Chase_mode,
	Frightened_mode
};

constexpr Position RED_GHOST_SCATTER_TARGET {CELL_SIZE * (MAP_WIDTH - 1), 0};
constexpr Position PINK_GHOST_SCATTER_TARGET {0, 0};
constexpr Position BLUE_GHOST_SCATTER_TARGET {CELL_SIZE * (MAP_WIDTH - 1), CELL_SIZE * (MAP_HEIGHT - 1)};
constexpr Position oORANGE_GHOST_SCATTER_TARGET {0, CELL_SIZE * (MAP_HEIGHT - 1)};

