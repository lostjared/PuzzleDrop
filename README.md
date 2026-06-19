# PuzzleDrop

![ScreenShot1](https://github.com/lostjared/PuzzleDrop/blob/main/screens/drop-ss1.png?raw=true "screenshot1")

![ScreenShot2](https://github.com/lostjared/PuzzleDrop/blob/main/screens/drop-ss2.png?raw=true "screenshot2")

PuzzleDrop is a Qt-based falling-block puzzle game built around color matching. Your goal is to stack and shift the falling 3-block piece so that the board forms matching sequences of `1-2-3` or `3-2-1` in a line. Matches can be horizontal, vertical, or diagonal. A flashing wildcard block can stand in for any color.

## How to Play

Start from the intro screen by choosing `Game > New Game`. While playing:

- `Left` / `Right`: move the piece sideways
- `Down`: drop the piece faster
- `A` / `S`: shift the colors within the current piece
- `Z` / `X`: rotate the piece

When a valid pattern is completed, those blocks clear and your line count increases. The game gets faster as your score rises.

## Menu Actions

- `Game > New Game`: start a new run, or reset the current one if a session is already active
- `Game > Quit Game`: stop the current session and return to the intro screen
- `Game > High Scores`: open the score table
- `Options > Difficulty`: choose `Easy`, `Medium`, or `Hard` before starting

## Scoring

Scores are recorded by total lines cleared. After a game ends, the score prompt asks for your name and stores the result locally using Qt `QSettings`.

## Build

Requires Qt 6 and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/bin/PuzzleDrop
```

## Project Files

- `main.cpp` initializes the application and styling
- `main_window.cpp` contains the game window and rendering
- `puzzle_drop.cpp` contains the core board and piece logic
- `high_scores.cpp` handles score storage and display
- `img/` contains bundled sprites and backgrounds
