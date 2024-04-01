#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>

#include "headers/global.hpp"
#include "headers/convert-sketch.hpp"
#include "headers/draw-map.hpp"
#include "headers/pacman.hpp"
#include "headers/red-ghost.hpp"
#include "headers/pink-ghost.hpp"
#include "headers/blue-ghost.hpp"
#include "headers/orange-ghost.hpp"

int main(){
    Pacman pacman;
	
	//Initial ghost positions.
	std::array<Position, 4> ghost_positions;

	// Ghosts
	RedGhost red_ghost;
	PinkGhost pink_ghost;
	BlueGhost blue_ghost;
	OrangeGhost orange_ghost;

 	std::array<std::string, MAP_HEIGHT> map_sketch = {
		" ################### ",
		" #........#........# ",
		" #o##.###.#.###.##o# ",
		" #.................# ",
		" #.##.#.#####.#.##.# ",
		" #....#...#...#....# ",
		" ####.### # ###.#### ",
		"    #.#   0   #.#    ",
		"#####.# ##=## #.#####",
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
	
	// Setting positions of the ghosts:
	red_ghost.set_position(ghost_positions[0].x,ghost_positions[0].y);
	pink_ghost.set_position(ghost_positions[1].x,ghost_positions[1].y);
	blue_ghost.set_position(ghost_positions[2].x,ghost_positions[2].y);
	orange_ghost.set_position(ghost_positions[3].x,ghost_positions[3].y);

	Position house_exit {ghost_positions[0].x, ghost_positions[0].y}; // Exit location to be used as initial target of ghosts within the house
	pink_ghost.set_home_exit(house_exit.x, house_exit.y);
	blue_ghost.set_home_exit(house_exit.x, house_exit.y);
	orange_ghost.set_home_exit(house_exit.x, house_exit.y);
   
	// (16 * 21 * 2 = 672) Width , (16 * 16 * 21 = 5376) Height for sf::VideoMode
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE), "Pac-Man Game", sf::Style::Close);

	// Sets the visible area of the window to the specified rectangle 
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));
	window.setFramerateLimit(60); // limit frame rate to 60fps

    sf::Event event; // Keeps track of events occurring within the window
	MovementMode movement_mode {MovementMode::Scatter_mode};

	// Game timers
	sf::Clock pacman_animation_clock;
	sf::Clock blue_animation_clock;
	sf::Clock red_animation_clock;
	sf::Clock pink_animation_clock;
	sf::Clock orange_animation_clock;
	sf::Clock game_play_time;

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

		if(game_play_time.getElapsedTime().asSeconds() > 7.0f && movement_mode != MovementMode::Chase_mode){
			movement_mode = MovementMode::Chase_mode;
			std::cout<<"Switching to chase Mode"<< std::endl;
		}

        pacman.draw(window,pacman_animation_clock);
		red_ghost.draw(window,red_animation_clock);
		pink_ghost.draw(window,pink_animation_clock);
		blue_ghost.draw(window,blue_animation_clock);
		orange_ghost.draw(window,orange_animation_clock);

        pacman.update(map,movement_mode);
		red_ghost.update(map,pacman,movement_mode);
		pink_ghost.update(map,pacman,movement_mode);
		blue_ghost.update(map,pacman,red_ghost.getPosition(),movement_mode);
		orange_ghost.update(map,pacman,movement_mode);

        draw_map(map,window);
        
        window.display();
    }
    

    return 0;
}