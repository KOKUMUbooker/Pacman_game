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

	sf::Font font;
	font.loadFromFile("./assets/fonts/Adventuring.ttf");

	// Ghosts
	RedGhost red_ghost;
	PinkGhost pink_ghost;
	BlueGhost blue_ghost;
	OrangeGhost orange_ghost;

	bool game_won {0};

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

	// Setting their home origins
	red_ghost.set_home(ghost_positions[0].x,ghost_positions[0].y);
	pink_ghost.set_home(ghost_positions[1].x,ghost_positions[1].y);
	blue_ghost.set_home(ghost_positions[2].x,ghost_positions[2].y);
	orange_ghost.set_home(ghost_positions[3].x,ghost_positions[3].y);

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
				case sf::Event::KeyPressed:
				{
					if(event.key.code == sf::Keyboard::Enter && (game_won || pacman.get_dead()))
					{
						std::cout << "Trying to reset game"<<std::endl;
						// Reset the game
						movement_mode = MovementMode::Scatter_mode;
						game_won = 0;
						map = convert_sketch(map_sketch, ghost_positions, pacman);
						red_ghost.reset();
						pink_ghost.reset();
						blue_ghost.reset();
						orange_ghost.reset();
						pacman.reset();
						game_play_time.restart();
						draw_map(map,window);
						continue;
					}
				}
			}
		}

        window.clear();

		if(game_play_time.getElapsedTime().asSeconds() > 7.0f && movement_mode != MovementMode::Chase_mode){
			movement_mode = MovementMode::Chase_mode;
			std::cout<<"Switching to chase Mode"<< std::endl;
		}

		// Check if there's atleast one pellet within the map to determine whether game has been won
		if(!game_won && pacman.get_dead() == 0)
		{
			game_won = 1;
			for (const std::array<Cell, MAP_HEIGHT>& column : map)
			{
				for (const Cell& cell : column)
				{
					if (cell == Cell::Pellet) 
					{
						game_won = 0;
						break;
					}
				}

				if (0 == game_won)
				{
					break;
				}
			}

			if(!game_won)
			{
				pacman.update(map,movement_mode);
				red_ghost.update(map,pacman,movement_mode);
				pink_ghost.update(map,pacman,movement_mode);
				blue_ghost.update(map,pacman,red_ghost.getPosition(),movement_mode);
				orange_ghost.update(map,pacman,movement_mode);

				pacman.draw(window,pacman_animation_clock);
				red_ghost.draw(window,red_animation_clock);
				pink_ghost.draw(window,pink_animation_clock);
				blue_ghost.draw(window,blue_animation_clock);
				orange_ghost.draw(window,orange_animation_clock);
       		 	draw_map(map,window);
			}
		}

		if (game_won)
		{
			// std::cout << "GAME WON 🎉🎉🎉🎉🎉🎉🎉"<<std::endl;
			sf::Text text("GAME WON\nHit Enter to play again",font,16);
			text.move(75.0f,168.0f);
			text.setFillColor(sf::Color::White);
			window.draw(text);
		}
		else if (pacman.get_dead())
		{
			std::cout << "GAME LOST 😵😵😵😵😵😵😵"<<std::endl;
			sf::Text text("GAME OVER",font,16);
			text.move(0.0f,0.0f);
			window.draw(text);
		}
    
        window.display();
    }
    

    return 0;
}