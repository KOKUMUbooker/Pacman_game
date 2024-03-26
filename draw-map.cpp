#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include "headers/draw-map.hpp"

void draw_map(const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, sf::RenderWindow &i_window)
{   

    for (unsigned char a = 0; a < MAP_HEIGHT; a++)
    {
        for (unsigned char b = 0; b < MAP_WIDTH; b++)
        {
            // Position it within the window using the CELL_SIZE as offsets, where a = x-axis,b= y-axis

            switch (i_map[a][b])
            {
                case Cell::Wall:
                {
                    // Rectangular shape of 16 * 16
                    sf::RectangleShape cell_shape(sf::Vector2f(CELL_SIZE,CELL_SIZE));       
                    cell_shape.setPosition(CELL_SIZE * a, CELL_SIZE * b);
                    cell_shape.setFillColor(sf::Color(36, 36, 255));
                    
                    i_window.draw(cell_shape);
                    break;
                }

                case Cell::Door:
                {
                    sf::RectangleShape cell_door(sf::Vector2f(CELL_SIZE,CELL_SIZE / 1.5 ));
                    cell_door.setPosition(CELL_SIZE * a, CELL_SIZE * b );
                    cell_door.setFillColor(sf::Color(138, 136, 137));
                    i_window.draw(cell_door);
                    
                    break;
                }

                case Cell::Pellet:
                {
                    sf::CircleShape circle_shape(CELL_SIZE / 8);
                    circle_shape.setPosition(CELL_SIZE * a + (CELL_SIZE / 2 - circle_shape.getRadius()), CELL_SIZE * b+ (CELL_SIZE / 2 - circle_shape.getRadius()));

                    i_window.draw(circle_shape);
                    break;
                }
            }

            
        }
        
    }
    
}