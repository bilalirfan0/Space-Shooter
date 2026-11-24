# Space Shooter

Space Shooter is a single-player 2D arcade game for Windows, built with C++17 and SFML 3 for a Programming Fundamentals semester project. Pilot your ship through three enemy formations, dodge falling bombs, and earn enough points to advance.

## Gameplay

Each level starts with three lives. Hitting an enemy or a bomb costs one life and sends your ship back to its starting position. You can start at any level from the main menu.

| Level | Enemies | Starting score | Goal |
| --- | ---: | ---: | --- |
| 1 | 6 | 0 | Reach 150 points to advance |
| 2 | 9 | 150 | Reach 330 points to advance |
| 3 | 15 | 330 | Reach 630 points to win |

The leaderboard shows your most recent completed run's score. Closing the game clears that score.

## Controls

| Screen | Key | Action |
| --- | --- | --- |
| Main menu | `S`, `2`, or `3` | Start at level 1, 2, or 3 |
| Main menu | `I` | Open instructions |
| Main menu | `L` | View the previous score |
| Main menu | `Q` | Quit |
| Playing | Arrow keys | Move the ship |
| Playing | `Space` | Fire once per press |
| Playing | `Esc` | Pause |
| Paused | `P` or `Esc` | Resume |
| Instructions or leaderboard | `Esc` | Return to the previous screen |
| Victory or game over | `L` | View the previous score |
| Victory or game over | `Q` | Return to the main menu |

## Run on Windows

On 64-bit Windows 10 or 11, extract the entire ZIP and double-click `Play.bat` in the extracted project folder. The game comes with its runtime DLLs, so you can play without installing SFML or a compiler.

Keep `Play.bat` next to the `build/` folder. The launcher starts the game from that folder so it can find its images and fonts.

## Build from source

1. Install a 64-bit MinGW-w64 C++ compiler and make sure `g++` is available on your `PATH`.
2. Set up SFML 3.1.0 with headers in `C:\SFML\include` and libraries in `C:\SFML\lib`, as expected by `run.bat`. Use the compiler that matches your SFML package, or build SFML with your own compiler. [SFML 3.1.0 downloads and compiler requirements](https://www.sfml-dev.org/download/sfml/3.1.0/)
3. Place the matching SFML and MinGW runtime DLLs in `build/` alongside the images and fonts.
4. From the folder containing `main.cpp`, run the following command. It builds `build/game.exe` and launches it from `build/`.

```powershell
.\run.bat
```

## Project files

| File or folder | Contents |
| --- | --- |
| `main.cpp` | Game loop, controls, menus, scoring, collisions, and level progression |
| `Play.bat` | Launches the bundled Windows game |
| `run.bat` | Windows build and launch script |
| `build/game.exe` and DLLs | Prebuilt Windows game and its runtime libraries |
| `build/img/` | Background, ship sprites, enemies, and projectiles |
| `build/font.otf` and `build/font2.otf` | Fonts used by the game |

## Team and credits

- **Bilal Irfan** — Asset loading, window and player setup, movement, and level progression.
- **Humail Tofeeq** — Enemy formations, bomb spawning, menus, and on-screen display.
- **Abdullah Bilal** — Input handling, firing, collision detection, and projectile movement.

Ship and laser artwork includes assets from Kenney. [Space Shooter Redux](https://opengameart.org/content/space-shooter-redux)

The game uses Lucy Rose PERSONAL by Ellen Luff and Pricedown by Ray Larabie. [Lucy Rose](https://ellenlufftype.com/lucy-rose/) · [Typodermic Fonts licensing](https://typodermicfonts.com/license/)

Graphics and windowing use the Simple and Fast Multimedia Library. [SFML license](https://www.sfml-dev.org/license/)
