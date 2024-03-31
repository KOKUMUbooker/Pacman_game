#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>

#include "headers/global.hpp"
#include "headers/utils.hpp"
#include "headers/red-ghost.hpp"
#include "headers/map-collision.hpp"

void RedGhost::draw(sf::RenderWindow &i_window, sf::Clock &animation_clock)
{
    sf::Texture texture;
    texture.loadFromFile("./assets/sprite_sheets/red_ghost.png");

    sf::IntRect rectSourceSprite(current_sprite_frame_edge,0,24,24);  // width = 24 , height = 24  
    sf::Sprite sprite(texture,rectSourceSprite);
    sprite.setScale(0.65f,0.65f);
    sprite.setPosition(position.x,position.y);

    // After a specified duration we change the sprite section currently in view
    // if(animation_clock.getElapsedTime().asSeconds() > 0.1f)
    // {
    //     if(rectSourceSprite.left == current_sprite_frame_edge)
    //     {
    //         rectSourceSprite.left = 0 ;
    //     }
    //     else{
    //         rectSourceSprite.left += SPRITE_GAME_CHARACTER_WIDTH ;
    //     }

    //     sprite.setTextureRect(rectSourceSprite);
    //     animation_clock.restart();
    // }

    i_window.draw(sprite);

    // Circle of radius = CELL_SIZE / 2 = 8
    // sf::CircleShape circle(CELL_SIZE / 2);
    // sf::CircleShape circle(CELL_SIZE / 2, 5);
    // circle.setFillColor(sf::Color::Red);
    // circle.setPosition(position.x,position.y);
    // i_window.draw(circle);
}

void RedGhost::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

void RedGhost::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,Pacman& i_pacman)
{
    // Check for collision in all directions
    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, GHOST_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - GHOST_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - GHOST_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, GHOST_SPEED + position.y, i_map);

    // Set the optimal path based on accessible direction
    set_optimal_direction(walls, direction , GHOST_SPEED ,position, i_pacman.getPosition());

    if(!walls[direction])
    {
        switch (direction)
        {
            case 0:
            {
                current_sprite_frame_edge = GHOST_RIGHT_FRAME_END;
                position.x += GHOST_SPEED;
                
                break;
            }
            case 1:
            {
                current_sprite_frame_edge = GHOST_UP_FRAME_END;
                position.y -= GHOST_SPEED;

                break;
            }
            case 2:
            {
                current_sprite_frame_edge = GHOST_LEFT_FRAME_END;
                position.x -= GHOST_SPEED;

                break;
            }
            case 3:
            {
                current_sprite_frame_edge = GHOST_DOWN_FRAME_END;
                position.y += GHOST_SPEED;
            }

        }
    }

    if (position.x <= -CELL_SIZE )
	{
        position.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
	}
	else if (position.x >= CELL_SIZE * MAP_WIDTH)
	{
        position.x = PACMAN_SPEED - CELL_SIZE;
	}
}

Position RedGhost::getPosition()
{
    return position;
};