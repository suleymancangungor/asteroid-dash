#ifndef ASTEROIDDASH_H
#define ASTEROIDDASH_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <algorithm>

#include "CelestialObject.h"
#include "LeaderboardEntry.h"
#include "Leaderboard.h"
#include "Player.h"

#define occupiedCellChar "██"
#define unoccupiedCellChar "▒▒"

using namespace std;

struct Bullet{
    int row = 0;
    int col = 0;
    bool out_of_bound = false;
    
    Bullet(int row, int col) : row(row), col(col) {}

    void move(int limit){
        col++;
        if (limit < col){
            out_of_bound = true;
        }
    }
};

// Class that encapsulates the game play internals
class AsteroidDash {
public:

    // Constructor to initialize the game
    AsteroidDash(const string &space_grid_file_name, const string &celestial_objects_file_name,
                 const string &leaderboard_file_name, const string &player_file_name, const string &player_name);

    // Destructor. Remove dynamically allocated member variables here
    virtual ~AsteroidDash();

    // 2D space_grid
    vector<vector<int> > space_grid;

    // Pointer to track the player instance
    Player *player = nullptr;

    // A reference to the head of the celestial objects linked list
    CelestialObject *celestial_objects_list_head = nullptr;

    // Current score of the game
    unsigned long current_score = 0;

    // Current game tick
    unsigned long game_time = 0;

    // Leaderboard file name to store and read the leaderboard from
    string leaderboard_file_name;

    // Leaderboard member variable
    Leaderboard leaderboard;

    // True if the game is over
    bool game_over = false;

    // Function to print the space_grid
    void print_space_grid() const;

    // Function to read the space grid from a file
    void read_space_grid(const string &input_file);

    // Function to read the player from a file
    void read_player(const string &player_file_name, const string &player_name);

    // Function to read celestial objects from a file
    // Reads the input file and calls the read_celestial_object() function for each celestial_object;
    void read_celestial_objects(const string &input_file);

    // Updates the grid based on player and celestial object states
    void update_space_grid();

    // Corresponds to the SHOOT command
    void shoot();

    int read_celestial_shape(vector<vector<bool>> &celestial_shape, string line);

    vector<Bullet*> bullets;

    bool handle_collision(CelestialObject* celestial, int row, int col, int celestial_row, int celestial_col);

    bool update_celestial_pos(CelestialObject* celestial);

    void move_celestial(CelestialObject* celestial);

    void delete_celestial(CelestialObject* celestial);

    // Deletes bullet that's hit an asteroid
    void delete_bullet_hit(Bullet* bullet_to_delete);

    // Delete bullets that are out of bounds
    void delete_bullet_oog();

    int control_celestial(CelestialObject* celestial);

    void move_bullets();

    void connect_prev_links(CelestialObject* celestial);

    void connect_new_prev_links(CelestialObject* celestial);
};


#endif // ASTEROIDDASH_H
