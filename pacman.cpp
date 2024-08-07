#include <SFML/Graphics.hpp>
#include <iostream>

#include "headers/global.hpp"
#include "headers/pacman.hpp"
#include "headers/map-collision.hpp"

Pacman::Pacman():dead{0},current_sprite_frame_edge_x_axis{0},current_sprite_frame_top_distance{0},energized_duration{0},lives{3}{}

void Pacman::draw(sf::RenderWindow &i_window, sf::Clock &animation_clock)
{
    sf::Texture texture;
    texture.loadFromFile("./assets/sprite_sheets/pacman.png");

    sf::IntRect rectSourceSprite(current_sprite_frame_edge_x_axis,current_sprite_frame_top_distance,16,16);  // width = 24 , height = 24  
    sf::Sprite sprite(texture,rectSourceSprite);
    sprite.setPosition(position.x,position.y);
    pacman_sprite = sprite;

    // After a specified duration we change the sprite section currently in view
    if(animation_clock.getElapsedTime().asSeconds() > 0.1f)
    {
        if(rectSourceSprite.left == 48)
        {
            rectSourceSprite.left = 0 ;
            current_sprite_frame_edge_x_axis = 0;
        }
        else{
            rectSourceSprite.left += PACMAN_SPRITE_GAME_CHARACTER_WIDTH ;
            current_sprite_frame_edge_x_axis += PACMAN_SPRITE_GAME_CHARACTER_WIDTH ;
        }

        sprite.setTextureRect(rectSourceSprite);
        animation_clock.restart();
    }

    i_window.draw(sprite);
}

void Pacman::set_position(short i_x,short i_y)
{
    position = {i_x,i_y};
}

unsigned char Pacman::getDirection()
{
    return direction;
}

void Pacman::reset()
{
    direction = 0;
    lives = 3;
    dead = 0;
    current_sprite_frame_edge_x_axis = 0;
    current_sprite_frame_top_distance = 0;
    position = home;
}

void Pacman::set_home(short i_x,short i_y)
{
    home = {i_x,i_y};
}

void Pacman::update(std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, MovementMode &cur_movement_mode)
{
    if(energized_duration > 0 && cur_movement_mode == MovementMode::Frightened_mode)
    {
        energized_duration --;
    }
    else if(energized_duration == 0 && cur_movement_mode == MovementMode::Frightened_mode)
    {
        energized_duration = 0;
        cur_movement_mode = MovementMode::Chase_mode;
        std::cout<< "Going back to Chase mode 👿👿👿👿👿👿👿👿" <<std::endl;
    }

    // 0 = Right, 1 = Up, 2 = left, 3 = Down
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, PACMAN_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - PACMAN_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - PACMAN_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, PACMAN_SPEED + position.y, i_map);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {   
        if(!walls[3])
        {
            direction = 3;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        if(!walls[2])
        {
            direction = 2;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if(!walls[0])
        {
            direction = 0;
        }
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        if(!walls[1])
        {
            direction = 1;
        }
    }

    if (!walls[direction])
	{
		switch (direction)
		{
			case 0:
            {
                current_sprite_frame_top_distance = PACMAN_RIGHT_FRAME_END;
                position.x += PACMAN_SPEED;
                
                break;
            }
            case 1:
            {
                current_sprite_frame_top_distance = PACMAN_UP_FRAME_END;
                position.y -= PACMAN_SPEED;

                break;
            }
            case 2:
            {
                current_sprite_frame_top_distance = PACMAN_LEFT_FRAME_END;
                position.x -= PACMAN_SPEED;

                break;
            }
            case 3:
            {
                current_sprite_frame_top_distance = PACMAN_DOWN_FRAME_END;
                position.y += PACMAN_SPEED;
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

    // map_collision returns 1 if energizer is eaten & i_collect_pellets is set to 1
    if(map_collision(1, 0, position.x, position.y, i_map))
    {
        cur_movement_mode = MovementMode::Frightened_mode;
        energized_duration = PACMAN_ENERGIZED_DURATION;
        std::cout<< "Changing mode to FRIGHTENED MODE 😖😖😖😖😖😖😖" <<std::endl;
    }
}

Position Pacman::getPosition()
{
    return position;
};

bool Pacman::get_dead()
{
    return dead;
};

void Pacman::set_dead(bool is_dead)
{
    dead = is_dead;
}

sf::Sprite Pacman::get_pacman_sprite()
{
    return pacman_sprite;
}

unsigned short Pacman::get_lives()
{
    return lives;
}

void Pacman::reduce_life()
{
    if(lives > 0)
    {
        lives --;
        position = home ;
        direction = Direction::Right;
    }
    else if(lives == 0)
    {
        dead = 1;
    }
}