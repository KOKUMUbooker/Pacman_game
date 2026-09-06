#include <array>
#include <cmath>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Rect2.hpp>

#include "global.hpp"
#include "map-collision.hpp"

bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
	bool output = 0;

	//Getting the exact position.
	float cell_x = i_x / static_cast<float>(CELL_SIZE);
	float cell_y = i_y / static_cast<float>(CELL_SIZE);

	//A ghost/Pacman can intersect 4 cells at most.
	for (unsigned char a = 0; a < 4; a++)
	{
		short x = 0;
		short y = 0;

		switch (a)
		{
			// case 0: 
			case Direction::Right: 
			{
				x = static_cast<short>(floor(cell_x));
				y = static_cast<short>(floor(cell_y));

				break;
			}
			// case 1: 
			case Direction::Up: 
			{
				x = static_cast<short>(ceil(cell_x));
				y = static_cast<short>(floor(cell_y));

				break;
			}
			// case 2: 
			case Direction::Left: 
			{
				x = static_cast<short>(floor(cell_x));
				y = static_cast<short>(ceil(cell_y));

				break;
			}
			// case 3: 
			case Direction::Down: 
			{
				x = static_cast<short>(ceil(cell_x));
				y = static_cast<short>(ceil(cell_y));
			}
		}

		//Making sure that the position is inside the map.
		if (0 <= x && 0 <= y && MAP_HEIGHT > y && MAP_WIDTH > x)
		{
			if (!i_collect_pellets) //Here we only care about the walls.
			{
				if (Cell::Wall == i_map[x][y])
				{
					output = 1;
				}
				else if (!i_use_door && Cell::Door == i_map[x][y])
				{
					output = 1;
				}
			}
			else //Here we only care about the collectables.
			{
				if (Cell::Energizer == i_map[x][y])
				{
					output = 1;

					i_map[x][y] = Cell::Empty;
				}
				else if (Cell::Pellet == i_map[x][y])
				{
					i_map[x][y] = Cell::Empty;
				}
			}
		}
	}

	return output;
}

// // Utilizes rectangular/bounding box collision detection
// bool sprite_collision(sf::Sprite sprite1, sf::Sprite sprite2)
// {
// 	bool is_colliding = 0;
// 	if(sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds()))
// 	{
// 		is_colliding = 1;
// 	}

// 	return is_colliding;
// }

bool sprite_collision(sf::Sprite sprite1, sf::Sprite sprite2)
{
	bool is_colliding = 0;

	// 1) Create the smallest rectangles that can hold the sprites - a way of getting radius
	sf::Rect2f sprite1_shape = sprite1.getGlobalBounds();
	sf::Rect2f sprite2_shape = sprite2.getGlobalBounds();

	// 2) Calculate the dy and dx based on positions of the 2 sprites
		// Get the center positions of either x and y axes for the sprites then difference of those centers are the dy and dx values 
	// sf::Sprite has no getPosition(); `position` is a public field.
	float dx = (sprite1.position.x + sprite1_shape.size.x) - (sprite2.position.x + sprite2_shape.size.x);
	float dy = (sprite1.position.y + sprite1_shape.size.y) - (sprite2.position.y + sprite2_shape.size.y);

	// 3) Get distance between the 2 sprite centers using pythagorean theorem
	float distance = std::sqrt(std::pow(dx,2) + std::pow(dy,2));

	// 4) If distance between the 2 sprites is less than the sum of their radii, collision has occurred
	if(distance < (sprite1_shape.size.x/2 + sprite2_shape.size.x/2))
	{
		is_colliding = 1;
	}
	
	return is_colliding;
}