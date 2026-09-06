#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <array>
#include "draw-map.hpp"

void draw_map(const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, sf::RenderTarget &i_window)
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
                    // RectangleShape is now built from a designated-initializer Data struct.
                    sf::RectangleShape cell_shape({
                        .position  = {static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b)},
                        .fillColor = sf::Color(36, 36, 255),
                        .size      = {CELL_SIZE, CELL_SIZE},
                    });
                    
                    i_window.draw(cell_shape);
                    break;
                }

                case Cell::Door:
                {
                    sf::RectangleShape cell_door({
                        .position  = {static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b)},
                        .fillColor = sf::Color(138, 136, 137),
                        .size      = {CELL_SIZE, CELL_SIZE / 1.5f},
                    });
                    i_window.draw(cell_door);

                    break;
                }

                case Cell::Pellet:
                {
                    // Radius is needed before the position can be computed (it depends on getRadius()),
                    // so construct with just the radius first, then set position on the resulting object.
                    sf::CircleShape circle_shape({.radius = CELL_SIZE / 8.f});
                    circle_shape.position = {CELL_SIZE * a + (CELL_SIZE / 2.f - circle_shape.getRadius()), CELL_SIZE * b + (CELL_SIZE / 2.f - circle_shape.getRadius())};

                    i_window.draw(circle_shape);
                    break;
                }
                case Cell::Energizer:
                {
                    sf::CircleShape circle_shape({.radius = CELL_SIZE / 4.f});
                    circle_shape.position = {CELL_SIZE * a + (CELL_SIZE / 2.f - circle_shape.getRadius()), CELL_SIZE * b + (CELL_SIZE / 2.f - circle_shape.getRadius())};

                    i_window.draw(circle_shape);
                    break;
                }
            }

            
        }
        
    }
    
}