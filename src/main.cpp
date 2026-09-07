#include <SFML/Graphics/GraphicsContext.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/ImGui/ImGuiContext.hpp>
#include <SFML/ImGui/IncludeImGui.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Path.hpp>
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

// Which screen is currently driving the frame.
enum class GameState
{
    MainMenu,
    Playing,
    Results
};

int main(){
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

	auto window = sf::RenderWindow::create(
		{
			.size  = {static_cast<unsigned int>(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE),
			          static_cast<unsigned int>((FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE)},
			.title = "Pac-Man Game",
		}).value();

	auto rtGame = sf::RenderTexture::create(
		{static_cast<unsigned int>(CELL_SIZE * MAP_WIDTH),
		 static_cast<unsigned int>(FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)}
	).value();

	window.setFramerateLimit(60);

	// ImGuiContext must be constructed after GraphicsContext (matches VRSFML's own examples).
	sf::ImGuiContext imGuiContext;
	sf::Clock imgui_delta_clock;

	MovementMode movement_mode {MovementMode::Scatter_mode};
	GameState game_state = GameState::MainMenu;
	bool last_game_won = false; // What to show on the Results screen.

	// Game timers
	sf::Clock pacman_animation_clock;
	sf::Clock blue_animation_clock;
	sf::Clock red_animation_clock;
	sf::Clock pink_animation_clock;
	sf::Clock orange_animation_clock;
	sf::Clock game_play_time;

	// Resets all game entities to their starting state and re-derives the map
	// from the sketch. Shared by "Play" (from the main menu) and
	// "Play Again" (from the results screen).
	const auto reset_game = [&]()
	{
		movement_mode = MovementMode::Scatter_mode;
		game_won = 0;
		pacman.reset();
		red_ghost.reset();
		pink_ghost.reset();
		blue_ghost.reset();
		orange_ghost.reset();
		game_play_time.restart();
		map = convert_sketch(map_sketch, ghost_positions, pacman);

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

		Position house_exit {ghost_positions[0].x, ghost_positions[0].y};
		pink_ghost.set_home_exit(house_exit.x, house_exit.y);
		blue_ghost.set_home_exit(house_exit.x, house_exit.y);
		orange_ghost.set_home_exit(house_exit.x, house_exit.y);
	};

	// Do the initial ghost placement once up front too, so Playing works
	// correctly even the very first time (before any reset_game() call).
	reset_game();

	bool running = true;

    while (running)
    {
        while (const sf::base::Optional event = window.pollEvent())
		{
			imGuiContext.processEvent(window, *event);

			if (event->is<sf::Event::Closed>())
			{
				running = false;
			}
		}

		if (!running)
		{
			break;
		}

		imGuiContext.update(window, imgui_delta_clock.restart());

		if (game_state == GameState::MainMenu)
		{
			ImGui::Begin("Pac-Man");
			if (ImGui::Button("Play"))
			{
				reset_game();
				game_state = GameState::Playing;
			}
			if (ImGui::Button("Quit"))
			{
				running = false;
			}
			ImGui::End();

			window.clear();
			imGuiContext.render(window);
			window.display();
		}
		else if (game_state == GameState::Playing)
		{
			rtGame.clear();

			if(!game_won && pacman.get_dead() == 0)
			{
				if(game_play_time.getElapsedTime().asSeconds() > 7.0f && movement_mode != MovementMode::Chase_mode && movement_mode != MovementMode::Frightened_mode){
					movement_mode = MovementMode::Chase_mode;
					std::cout<<"Switching to chase Mode"<< std::endl;
				}

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

			// Win/loss just occurred this frame -- hand off to the Results screen.
			if (game_won || pacman.get_dead())
			{
				last_game_won = game_won;
				game_state = GameState::Results;
			}

			rtGame.display();

			window.clear();
			window.draw(rtGame.getTexture(), {.scale = {static_cast<float>(SCREEN_RESIZE), static_cast<float>(SCREEN_RESIZE)}});
			imGuiContext.render(window);
			window.display();
		}
		else if (game_state == GameState::Results)
		{
			ImGui::Begin("Results");
			ImGui::Text(last_game_won ? "You won!" : "You lost!");
			if (ImGui::Button("Play Again"))
			{
				reset_game();
				game_state = GameState::Playing;
			}
			if (ImGui::Button("Back to Main Menu"))
			{
				game_state = GameState::MainMenu;
			}
			if (ImGui::Button("Quit"))
			{
				running = false;
			}
			ImGui::End();

			window.clear();
			imGuiContext.render(window);
			window.display();
		}
    }

    return 0;
}