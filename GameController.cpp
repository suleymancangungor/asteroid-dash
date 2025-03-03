#include "GameController.h"

// Simply instantiates the game
GameController::GameController(
        const string &space_grid_file_name,
        const string &celestial_objects_file_name,
        const string &leaderboard_file_name,
        const string &player_file_name,
        const string &player_name

) {
    game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                            player_name);
    // TODO: Your code here, if you want to perform extra initializations
}

// Reads commands from the given input file, executes each command in a game tick
void GameController::play(const string &commands_file) {
    ifstream file(commands_file);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << commands_file << endl;
        exit(1);
    }
    string line;
    while(getline(file, line)){
        if (!game->game_over){
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());
            if (line == "MOVE_UP"){
                game->player->move_up();
            } else if (line == "MOVE_DOWN"){
                game->player->move_down(game->space_grid.size());
            } else if (line == "MOVE_LEFT"){
                game->player->move_left();
            } else if (line == "MOVE_RIGHT"){
                game->player->move_right(game->space_grid[0].size());
            } else if (line == "SHOOT"){
                game->shoot();
            } else if (line == "PRINT_GRID"){
            } else if (line == "NOP") {
            } else {
                cout << "Unknown command: " << line << endl;
            }
            game->game_time++;
            game->current_score++;
            game->update_space_grid();
        }
        game->print_space_grid();
        if (game->game_over){
            break;
        }
    }
    if (!(game->game_over)){
        cout << "GAME FINISHED! No more commands!" << endl;
        game->game_over = true;
    } else {
        cout << "GAME OVER!" << endl;
    }
    game->update_space_grid();
    game->print_space_grid();
    game->leaderboard.insert(new LeaderboardEntry(game->current_score, time(nullptr), game->player->player_name));
    game->leaderboard.write_to_file(game->leaderboard_file_name);
    game->leaderboard.print_leaderboard();
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController() {
    delete game;
}
