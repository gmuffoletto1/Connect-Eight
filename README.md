# Connect-Eight

A 50x50 Connect Eight game written in C++ with an SFML GUI. Red is the human
player and Yellow is controlled by a minimax AI.

## Requirements

- CMake 3.16+
- A C++17 compiler
- SFML 2.5+ development libraries

On Ubuntu/Debian:

```sh
sudo apt-get install libsfml-dev
```

## Build and run

```sh
cmake -S . -B build
cmake --build build
./build/connect_eight
```

## Run tests

```sh
ctest --test-dir build --output-on-failure
```

## Controls

- Left click a column to drop a red piece.
- Right click a yellow piece to use one of Red's removals.
- Click **Undo** or press `U` to undo the previous turn.
- Click **New Game** or press `N` to reset.
- Press `Esc` to quit.
