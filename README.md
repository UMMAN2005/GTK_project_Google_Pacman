# Google Pacman Clone

This project is a clone of the classic Google Pacman game written in C using the GTK library.

## Features

- Replicates the gameplay mechanics of the original Pacman game.
- Utilizes the GTK library for creating the graphical user interface.
- Cross-platform compatibility.

### Game Features

- **Classic Gameplay**: Experience the iconic Pacman gameplay with maze navigation, dot collection, and ghost avoidance.
- **Power Pellets**: Consume power pellets to temporarily turn the tables on the ghosts, allowing Pacman to eat them.
- **Fruit Bonuses**: Encounter fruit bonuses that appear periodically, providing additional points when collected.
- **Ghost Behavior**: Ghosts exhibit various behaviors, including chasing, patrolling, and scattering, adding depth to the gameplay.
- **Responsive Controls**: Control Pacman with precision using responsive keyboard controls, ensuring smooth gameplay.

### Todo

- **Multiple Levels**: Progress through multiple levels of increasing difficulty, each with unique mazes and challenges.
- **Customizable Settings**: Adjust game settings such as sound volume, difficulty level, and control sensitivity to tailor the experience to your preferences.
- **Pause and Resume**: Pause the game at any time to take a break or attend to other tasks, and resume from where you left off.
- **Game Over Screen**: View a game over screen with your final score and the option to restart the game or exit to the main menu.
- **Animated Characters**: Enjoy animated sprites for Pacman, ghosts, and other game elements, bringing the game to life.
- **Sound Effects**: Immerse yourself in the game with authentic sound effects for Pacman movement, dot collection, and ghost interactions.
- **High Scores**: Track your highest scores across multiple gameplay sessions to compete with yourself and others.

## Files

- `.gitattributes`: Git attributes file.
- `.gitignore`: Git ignore file.
- `gameplay.mp4`: Mini gameplay video.
- `app.c`: Source code file containing the implementation of the game logic.
- `app.h`: Header file containing function declarations and data structures.
- `app.o`: Object file generated from compiling `app.c`.
- `LICENSE`: License file specifying the terms of use for the project.
- `main.c`: Source code file containing the main function and GTK initialization.
- `main.o`: Object file generated from compiling `main.c`.
- `Makefile`: Makefile for building the project.
- `Dockerfile`: Dockerfile for building the Docker image.
- `pacman.exe`: Executable file for the Pacman game.
- `style.css`: CSS file for styling the game interface.
- `Many .dll files`: Dynamic link library files required for running the executable.

## How to Build

1. Ensure you have GTK installed on your system.
2. Run `make` command in the project directory.
3. This will compile the source files and generate the executable `pacman.exe`.

## How to Play

1. After building the project, run `pacman.exe`.
2. Use arrow keys to control Pacman:
   - Up arrow: Move Pacman up.
   - Down arrow: Move Pacman down.
   - Left arrow: Move Pacman left.
   - Right arrow: Move Pacman right.
3. Collect all the dots while avoiding ghosts to advance to the next level.

## License

This project is licensed under the [MIT License](LICENSE).
