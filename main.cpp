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
		window.clear();

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
						std::cout << "Resetting the game 🔁🔁🔁🔁🔁🔁🔁🔁"<<std::endl;
						// Reset the game
						movement_mode = MovementMode::Scatter_mode;
						game_won = 0;
						pacman.reset();
						red_ghost.reset();
						pink_ghost.reset();
						blue_ghost.reset();
						orange_ghost.reset();
						game_play_time.restart();
						map = convert_sketch(map_sketch, ghost_positions, pacman);
						draw_map(map,window);
						continue;
					}
				}
			}
		}


		if(!game_won && pacman.get_dead() == 0)
		{
			// Timer to determine when to switch movement mode
			if(game_play_time.getElapsedTime().asSeconds() > 7.0f && movement_mode != MovementMode::Chase_mode && movement_mode != MovementMode::Frightened_mode){
				movement_mode = MovementMode::Chase_mode;
				std::cout<<"Switching to chase Mode"<< std::endl;
			}

			game_won = 1;
			// Check if there's atleast one pellet within the map to determine whether game has been won
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
				sf::Text text("LIVES : ",font,10);
				text.move(CELL_SIZE,BOTTOM_SCREEN_Y_AXIS);
				text.setFillColor(sf::Color::Red);
				window.draw(text);

				sf::Texture texture;
    			texture.loadFromFile("./assets/heart.png");
				float initial_x_position = 54.0f; 
				for (short i = 1; i <= pacman.get_lives() ; i ++)
				{
					sf::Sprite sprite(texture);
					sprite.setScale(0.025f,0.025f);
					if(i > 1) initial_x_position = initial_x_position + CELL_SIZE;
					sprite.setPosition(initial_x_position,BOTTOM_SCREEN_Y_AXIS); 
					window.draw(sprite);
				}

				pacman.draw(window,pacman_animation_clock);
				red_ghost.draw(window,red_animation_clock,movement_mode);
				pink_ghost.draw(window,pink_animation_clock,movement_mode);
				blue_ghost.draw(window,blue_animation_clock,movement_mode);
				orange_ghost.draw(window,orange_animation_clock,movement_mode);

				pacman.update(map,movement_mode);
				red_ghost.update(map,pacman,movement_mode);
				pink_ghost.update(map,pacman,movement_mode);
				blue_ghost.update(map,pacman,red_ghost.getPosition(),movement_mode);
				orange_ghost.update(map,pacman,movement_mode);

       		 	draw_map(map,window);
			}
		}
		else if (game_won)
		{
			// std::cout << "GAME WON 🎉🎉🎉🎉🎉🎉🎉"<<std::endl;
			sf::Text text("GAME WON\nHit Enter to play again",font,16);
			text.move(75.0f,168.0f);
			text.setFillColor(sf::Color::Yellow);
			window.draw(text);
		}
		else if (pacman.get_dead())
		{
			// std::cout << "GAME LOST 😵😵😵😵😵😵😵"<<std::endl;
			sf::Text text("GAME OVER\nHit Enter to play again",font,16);
			text.setFillColor(sf::Color::Red);
			text.move(75.0f,170.0f);
			window.draw(text);
		}
    
        window.display();
    }
    

    return 0;
}