# Project Readme

## Compilation Instructions

To compile the project, ensure you have `g++` and the SFML library installed on your system.

Compile the project using the following command:

g++ \*.cpp -lsfml-graphics -lsfml-window -lsfml-system -o pacman

To run it, execute the `pacman` file:

./pacman

## Demo Samples

1. Sample game play
   <!-- ![Sample game play](./readme_assets/game_demo.gif) -->
   <img src="./readme_assets/game_demo.gif" alt="drawing" width="50%" height="50%"/>

2. Game won screen
   <!-- ![Game won screen](./readme_assets/game_won_screen.png) -->
   <img src="./readme_assets/game_won_screen.png" alt="drawing" width="50%" height="50%"/>

3. Game lost screen
   <!-- ![Game lost screen](./readme_assets/game_over_screen.png) -->
   <img src="./readme_assets/game_over_screen.png" alt="drawing" width="50%" height="50%"/>

## Game Play Mechanics

The gameplay is centered around Pac-Man consuming all pellets while avoiding pursuit by the ghosts. Each of the ghosts has 3 modes:

### i) Scatter Mode

Default starting mode for all ghosts whereby they target their respective corners within the game map.

• The target corners are inaccessible and the ghosts cannot stop moving or reverse direction.

• They are forced to continue past the target but will turn back towards it as soon as possible.

• This results in each ghost's path eventually becoming a fixed loop in their corner.

• If left in Scatter mode, each ghost would remain in its loop indefinitely.

• However, the game's configuration limits the mode to the first 7 seconds of the game which later on switches to chase mode unless interrupted by Pac-Man eating an energizer causing a switch to frightened mode.

![Ghosts Scatter Mode](./readme_assets/ghosts-scatter-mode.png)
_Each ghost's target tile and eventual looping path, color-coded to match their own color._

### ii) Frightened Mode

Occurs when Pac-Man consumes an energizer resulting in slow movement of the ghosts as well as switching color to dark blue.

• During this mode, Pac-Man can openly attack any of the ghosts without losing a life.

### iii) Chase Mode

Occurs automatically after the 7 seconds of the scatter mode have elapsed.

• This causes the ghosts to target Pac-Man each in their unique way.

• During this mode, Pac-Man is vulnerable to the ghosts and contact between Pac-Man and any of the ghosts will cost the player a life.

## Game Entities

### 1. Pac-Man

• Is the controllable yellow character that is under the control of the player.

• Control is automatic such that provided there is no wall Pac-Man will progressively move in that current direction until he encounters an obstacle.

• Upon arriving at a junction (map regions with more than 1 pathway), the player can switch Pac-Man's current direction by hitting a different arrow key.

Pac-Man's control scheme is as follows:
• Up arrow key: move up

• Down arrow key: move downwards

• Right arrow key: move to the right

• Left arrow key: move to the left

### 2. Ghosts House

• Only one ghost (the red ghost) begins in the actual maze, while the others are inside a small area in the middle of the maze, often referred to as the "ghost house".

• The pink, blue, and orange ghosts will only return to this area if they are eaten by an energized Pac-Man (when mode = Frightened mode), or as a result of their positions being reset when Pac-Man dies.

• The ghost house is otherwise inaccessible, and is not a valid area for Pac-Man or the ghosts to move into once.

### 3. Target Tiles

• Much of Pac-Man's design and mechanics revolve around the idea of the board being split into tiles.

• As an example of the impact of tiles, a ghost is considered to have caught Pac-Man when it occupies the same tile as him.

• The large majority of the time, each ghost has a specific tile that it is trying to reach, and its behavior revolves around trying to get to that tile from its current one.

### 4. Ghosts

• These are the game enemies: four little ghost-shaped monsters, each of them a different color - blue, yellow, pink, and red.

#### Individual Ghost Personalities

The only differences between the ghosts are their methods of selecting target tiles in Chase and Scatter modes.

![Ghost Personalities](./readme_assets/ghost-personalities.png)
_Character and nicknames of the ghosts in English and Japanese._

## Code Implementation for Game Play Mechanics

### i) Pacman and Ghost Collision

- Implemented using circle collision detection.
- Involves having a circle surround the sprites (file images of the various characters) of the two entities.
- Calculates the Euclidean distance between the two entities.
- Collision is detected if the distance between the two sprites is less than the sum of the two circle radii.

### ii) Setting Optimal Direction before Movement

- Implemented using a greedy search algorithm:
  - Checks immediate surroundings for walls in all four directions.
  - Keeps track of the Euclidean distance from each unblocked cell to the target in a map in key-value pairs.
  - If there are multiple accessible paths, selects the direction with the lowest Euclidean distance.
  - If only one path is accessible, sets the direction as the value of the key of the first element in the map.
  - Clears elements of the map after setting the optimal direction.

### iii) Animations

- Implemented using sprite sheet PNGs containing multiple images.
- Displays specific sections of the PNG image based on time intervals.
- The cycle of image portions starts from a specific point, goes up to a designated end, and then returns to the origin.

### iv) Chase Mode for the Red Ghost

- Sets the red ghost's target as Pacman's position.
- Gets the optimal direction using Pacman as the target.
- Increments the ghost's x or y position accordingly.

### v) Chase Mode for the Pink Ghost

- Calculates four tiles in Pacman's direction to get the pink ghost's target.
- Sets the optimal direction using this new target and increments the ghost's x or y position.

### vi) Chase Mode for the Blue Ghost

- Calculates two tiles in Pacman's direction to get the red ghost's initial target.
- Calculates the Euclidean distance between the red ghost and the initial target (two tiles ahead of Pacman).
- Doubles this distance to get the blue ghost's target.
  - Calculates the angle between this vector and the x-axis using the red ghost and the initial target coordinates.
  - Computes dy and dx based on the angle and offset the red ghost's coordinates to get the vector's end coordinates.
- Sets the optimal direction using this new target and increments the ghost's x or y position.

### vii) Chase Mode for the Orange Ghost

- Calculates the Manhattan distance from the orange ghost to Pacman.
- If the distance is greater than eight cells, uses the red ghost's targeting approach; otherwise, targets its corner from the scatter mode.
- Sets the optimal direction and increments the ghost's x or y position accordingly.

## Technologies Used

• C++

• SFML

## Content Reference

For more details on the behavior of the ghosts in Pac-Man, visit [Understanding Pac-Man Ghost Behavior](https://gameinternals.com/understanding-pac-man-ghost-behavior).