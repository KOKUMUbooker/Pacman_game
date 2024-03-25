#include <SFML/Graphics.hpp>
#include <array>
#include <ctime>

#include "headers/global.hpp"
#include "headers/convert-sketch.hpp"
#include "headers/draw-map.hpp"
#include "headers/pacman.hpp"
#include "headers/ghost.hpp"

int main(){
    Pacman pacman;
	
	//Initial ghost positions.
	std::array<Position, 4> ghost_positions;

	Ghost ghost;

 	std::array<std::string, MAP_HEIGHT> map_sketch = {
		" ################### ",
		" #........#........# ",
		" #o##.###.#.###.##o# ",
		" #.................# ",
		" #.##.#.#####.#.##.# ",
		" #....#...#...#....# ",
		" ####.### # ###.#### ",
		"    #.#   0   #.#    ",
		"#####.# ##### #.#####",
		"     .  #123#  .     ",
		"#####.# ##### #.#####",
		"    #.#       #.#    ",
		" ####.# ##### #.#### ",
		" #........#........# ",
		" #.##.###.#.###.##.# ",
		" #o.#.....P.....#.o# ",
		" ##.#.#.#####.#.#.## ",
		" #....#...#...#....# ",
		" #.######.#.######.# ",
		" #.................# ",
		" ################### "
	};

    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map = convert_sketch(map_sketch,ghost_positions,pacman);
	//TODO: TO BE REMOVED LATER :
	ghost.set_position(ghost_positions[0].x,ghost_positions[0].y);

    // (16 * 21 * 2 = 672) Width , (16 * 16 * 21 = 5376) Height for sf::VideoMode
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE), "Pac-Man Game", sf::Style::Close);

	// Sets the visible area of the window to the specified rectangle 
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));
	window.setFramerateLimit(60); // limit frame rate to 60fps

    sf::Clock clock; // For measuring elapsed time
    sf::Event event; // Keeps track of events occurring within the window

    // Game loop
    while (window.isOpen())
    {
        // Handle events
        while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
				{
					//Making sure the player can close the window.
					window.close();
				}
			}
		}

        window.clear();
        pacman.draw(window);
		ghost.draw(window);
        pacman.update(map);
		ghost.update(map);
        draw_map(map,window);
        
        window.display();
    }
    

    return 0;
}