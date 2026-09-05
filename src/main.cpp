#include <SFML/Graphics/GraphicsContext.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Path.hpp>
#include <SFML/System/Time.hpp>
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
   	auto graphicsContextOpt = sf::GraphicsContext::create();
    if (!graphicsContextOpt.hasValue()) { std::cout << "FAILED: GraphicsContext::create" << std::endl; return 1; }
    auto graphicsContext = std::move(graphicsContextOpt).value();

    Pacman pacman;
	
	//Initial ghost positions.
	std::array<Position, 4> ghost_positions;

	auto fontOpt = sf::Font::openFromFile(asset_path("./assets/fonts/Pixel NES.otf"));
    if (!fontOpt.hasValue()) { std::cout << "FAILED: Font::openFromFile" << std::endl; return 1; }
	const auto font = std::move(fontOpt).value();

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
   
	// The window itself is created at the *scaled* size (what the player actually sees)...
	auto windowOpt = sf::RenderWindow::create(
		{
			.size  = {static_cast<unsigned int>(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE),
			          static_cast<unsigned int>((FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE)},
			.title = "Pac-Man Game",
		});
    if (!windowOpt.hasValue()) { std::cout << "FAILED: RenderWindow::create" << std::endl; return 1; }
	auto window = std::move(windowOpt).value();

	// ...while all game content is drawn into this offscreen texture at the
	// original *unscaled* resolution, exactly as it always has been. This
	// replaces the old setView(...) call: instead of remapping coordinates
	// via a view, we render at native scale into rtGame and then blit that
	// (scaled up) onto the window at the very end of each frame.
	auto rtGameOpt = sf::RenderTexture::create(
		{static_cast<unsigned int>(CELL_SIZE * MAP_WIDTH),
		 static_cast<unsigned int>(FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)}
	);
    if (!rtGameOpt.hasValue()) { std::cout << "FAILED: RenderTexture::create" << std::endl; return 1; }
	auto rtGame = std::move(rtGameOpt).value();

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
		rtGame.clear();

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
					draw_map(map,rtGame);
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
				rtGame.draw(text);

				sf::Text text2(font, {.string = sf::Utf8String("GHOST MODE : " + get_ghost_mode(movement_mode)), .characterSize = 10});
				text2.position = {CELL_SIZE * 10, BOTTOM_SCREEN_Y_AXIS + 2.0f};
				text2.setFillColor(sf::Color::Yellow);
				rtGame.draw(text2);

				const auto texture = sf::Texture::loadFromFile(asset_path("./assets/heart.png")).value();
				float initial_x_position = 54.0f; 
				for (short i = 1; i <= pacman.get_lives() ; i ++)
				{
					sf::Sprite sprite{.scale = {0.025f, 0.025f}};
					if(i > 1) initial_x_position = initial_x_position + CELL_SIZE;
					sprite.position = {initial_x_position, BOTTOM_SCREEN_Y_AXIS + 2.0f}; 
					rtGame.draw(sprite, {.texture = &texture});
				}

				pacman.draw(rtGame,pacman_animation_clock);
				red_ghost.draw(rtGame,red_animation_clock,movement_mode);
				pink_ghost.draw(rtGame,pink_animation_clock,movement_mode);
				blue_ghost.draw(rtGame,blue_animation_clock,movement_mode);
				orange_ghost.draw(rtGame,orange_animation_clock,movement_mode);

				pacman.update(map,movement_mode);
				red_ghost.update(map,pacman,movement_mode);
				pink_ghost.update(map,pacman,movement_mode);
				blue_ghost.update(map,pacman,red_ghost.getPosition(),movement_mode);
				orange_ghost.update(map,pacman,movement_mode);

       		 	draw_map(map,rtGame);
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
			rtGame.draw(text);
			rtGame.draw(text2);
		}
		else if (pacman.get_dead())
		{
			// std::cout << "GAME LOST 😵😵😵😵😵😵😵"<<std::endl;
			const auto texture = sf::Texture::loadFromFile(asset_path("./assets/game-over.png")).value();
			sf::Sprite sprite{.position = {40.0f, (CELL_SIZE * MAP_HEIGHT) / 5.0f}};
			rtGame.draw(sprite, {.texture = &texture});
			
			sf::Text text(font, {.string = "Hit Enter to play again", .characterSize = 12});
			text.setFillColor(sf::Color::Red);
			text.position = {CELL_SIZE * 4, (CELL_SIZE * MAP_HEIGHT) / 1.35f};
			rtGame.draw(text);
		}
    
        rtGame.display();

        // Blit the unscaled game content onto the window, scaled up by SCREEN_RESIZE.
        // DrawTextureSettings' default textureRect{} means "the full texture"
        // (unlike sf::Sprite, where an empty textureRect means zero-size/nothing).
        window.clear();
        window.draw(rtGame.getTexture(), {.scale = {static_cast<float>(SCREEN_RESIZE), static_cast<float>(SCREEN_RESIZE)}});
        window.display();
    }
    

    return 0;
}