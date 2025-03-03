# Asteroid Dash

Asteroid Dash is a thrilling, grid-based space adventure where strategy meets action. Navigate your spacecraft through an onslaught of asteroids, manage your limited resources, and power up to stay in the fight!

## Overview

In **Asteroid Dash**, the gameplay unfolds on a dynamic grid where:
- **Asteroids** appear from the right and steadily move leftward toward your spacecraft.
- Your **spacecraft** can fire to eliminate threats, but with **limited ammunition**, every shot counts.
- **Power-ups** are available to replenish ammo or restore lives, adding a strategic layer to the gameplay.
- The grid updates in real-time, moving and rotating asteroids as they get damaged and responding to your actions.

## Gameplay Mechanics

- **Grid-Based Movement**: The game environment is structured as a grid, creating a unique spatial challenge.
- **Tactical Resource Management**: With finite ammo, you must decide when to shoot or dodge.
- **Dynamic Environment**: Asteroids not only move but also rotate as they take damage, making every encounter unpredictable.
- **Power-Ups**: Collect items that either replenish your ammo or restore your lives, keeping the action engaging.

## Run Configuration

### Compilation

To compile the project, use the following command. (Note: our test files will be used in place of `main.cpp`.)

```bash```
$ g++ -std=c++11 -g main.cpp AsteroidDash.h AsteroidDash.cpp CelestialObject.h CelestialObject.cpp GameController.h GameController.cpp LeaderboardEntry.h LeaderboardEntry.cpp Leaderboard.h Leaderboard.cpp Player.h Player.cpp -o AsteroidDash

### Execution

$ ./AsteroidDash space_grid.dat celestial_objects.dat player.dat commands.dat leaderboard.txt AsteroidBuster

## File Descriptions
- space_grid.dat: Contains the grid layout for the game space.
- celestial_objects.dat: Holds data for various celestial objects, including asteroids.
- player.dat: Stores player's position and shape information.
- commands.dat: Lists the game commands that drive gameplay.
- leaderboard.txt: The file where the game writes the leaderboard.
- AsteroidBuster: A parameter for player name.
