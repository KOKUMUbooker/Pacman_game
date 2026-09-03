#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>

#include "global.hpp"
#include "convert-sketch.hpp"
#include "draw-map.hpp"
#include "pacman.hpp"
#include "red-ghost.hpp"
#include "pink-ghost.hpp"
#include "blue-ghost.hpp"
#include "orange-ghost.hpp"
#include "utils.hpp"
#include "asset-path.hpp"

int main(){
    // VRSFML requires a graphics context to exist before any window,
    // texture, or font is created.
    auto graphicsContext = sf::GraphicsContext::create().value();

    Pacman pacman;
	
	//Initial ghost positions.
	std::array<Position, 4> ghost_positions;

	const auto font = sf::Font::openFromFile("./assets/fonts/Pixel NES.otf").value();

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
	// RenderWindow is now created via a static factory taking a Settings
	// (WindowSettings) struct, returning base::Optional<RenderWindow>.
	auto window = sf::RenderWindow::create(
		{
			.size  = {static_cast<unsigned int>(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE),
			          static_cast<unsigned int>((FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE)},
			.title = "Pac-Man Game",
		}).value();

	// No custom view needed: the window is created at exactly the size we want
	// to draw into, and RenderStates::view defaults to one computed from the
	// render target's size, so this replaces the old setView(...) call.
	window.setFramerateLimit(60); // limit frame rate to 60fps

	MovementMode movement_mode {MovementMode::Scatter_mode};

	// Game timers
	sf::Clock pacman_animation_clock;
	sf::Clock blue_animation_clock;
	sf::Clock red_animation_clock;
	sf::Clock pink_animation_clock;
	sf::Clock orange_animation_clock;
	sf::Clock game_play_time;

	// There's no window.isOpen()/close() in VRSFML; the loop runs until we
	// see a Closed event, at which point we flip this flag and exit.
	bool running = true;

    // Game loop
    while (running)
    {
		window.clear();

        // Handle events
        while (const sf::base::Optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				//Making sure the player can close the window.
				running = false;
			}
			else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
			{
				if(key_pressed->code == sf::Keyboard::Key::Enter && (game_won || pacman.get_dead()))
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

		if (!running)
		{
			break;
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
				sf::Text text(font, {.string = "LIVES ", .characterSize = 10});
				text.position = {CELL_SIZE, BOTTOM_SCREEN_Y_AXIS + 2.0f};
				text.setFillColor(sf::Color::Red);
				window.draw(text);

				sf::Text text2(font, {.string = "GHOST MODE : " + get_ghost_mode(movement_mode), .characterSize = 10});
				text2.position = {CELL_SIZE * 10, BOTTOM_SCREEN_Y_AXIS + 2.0f};
				text2.setFillColor(sf::Color::Yellow);
				window.draw(text2);

				const auto texture = sf::Texture::loadFromFile(asset_path("./assets/heart.png")).value();
				float initial_x_position = 54.0f; 
				for (short i = 1; i <= pacman.get_lives() ; i ++)
				{
					// sf::Sprite no longer stores a texture; it's a plain
					// aggregate, and the texture is supplied at draw time.
					sf::Sprite sprite{.scale = {0.025f, 0.025f}};
					if(i > 1) initial_x_position = initial_x_position + CELL_SIZE;
					sprite.position = {initial_x_position, BOTTOM_SCREEN_Y_AXIS + 2.0f}; 
					window.draw(sprite, {.texture = &texture});
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
			sf::Text text(font, {.string = "GAME WON", .characterSize = 32});
			sf::Text text2(font, {.string = "Hit Enter to play again", .characterSize = 16});
			text.position = {55.0f, 168.0f};
			text2.position = {25.0f, 200.0f};
			text.setFillColor(sf::Color::Yellow);
			text2.setFillColor(sf::Color::Yellow);
			window.draw(text);
			window.draw(text2);
		}
		else if (pacman.get_dead())
		{
			// std::cout << "GAME LOST 😵😵😵😵😵😵😵"<<std::endl;
			const auto texture = sf::Texture::loadFromFile(asset_path("./assets/game-over.png")).value();
			sf::Sprite sprite{.position = {40.0f, (CELL_SIZE * MAP_HEIGHT) / 5.0f}};
			window.draw(sprite, {.texture = &texture});
			
			sf::Text text(font, {.string = "Hit Enter to play again", .characterSize = 12});
			text.setFillColor(sf::Color::Red);
			text.position = {CELL_SIZE * 4, (CELL_SIZE * MAP_HEIGHT) / 1.35f};
			window.draw(text);
		}
    
        window.display();
    }
    

    return 0;
}