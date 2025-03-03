#include "AsteroidDash.h"


// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)

        : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard()) {
    read_player(player_file_name, player_name);  // Initialize player using the player.dat file
    read_space_grid(space_grid_file_name);  // Initialize the grid after the player is loaded
    read_celestial_objects(celestial_objects_file_name);  // Load celestial objects
    leaderboard.read_from_file(leaderboard_file_name);
}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file) {
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << input_file << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        istringstream iss(line);
        vector<int> row;
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        if (!row.empty()) {
            space_grid.push_back(row);
        }
    }
}

// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name) {
    ifstream file(player_file_name);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << player_file_name << endl;
        exit(1);
    }
    string line;
    vector<vector<bool>> spacecraft_shape;
    int shape_row, shape_col;
    file >> shape_row >> shape_col;
    while (getline(file, line)) {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        istringstream iss(line); 
        if (line.empty()) {
            continue;
        }
        vector<bool> row;
        for(char c : line){
            if(c == '1'){
                row.push_back(true);
            } else if (c == '0'){
                row.push_back(false);
            }
        }
        spacecraft_shape.push_back(row);
    }
    player = new Player(spacecraft_shape, shape_row, shape_col, player_name, 10, 3);
}   

// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file) {
    ifstream file(input_file);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << input_file << endl;
        exit(1);
    }
    vector<vector<bool>> celestial_shape;
    ObjectType type = NONE;
    int row;
    int time_of_appearance;
    string effect;
    string line;
    bool create = false;
    int full_cell_count = 0;
    while(getline(file, line)){
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        istringstream iss(line);
        if(line.empty()){
            continue;
        } else if (line.find('s') != string::npos){
            row = stoi(line.substr(2));
        } else if (line.find('t') != string::npos){
            time_of_appearance = stoi(line.substr(2));
            if (type == ASTEROID){
                create = true;
            }
        } else if (line.find('e') != string::npos){
            effect = line.substr(2);
            if (effect == "ammo"){
                type = AMMO;
            } else if (effect == "life"){
                type = LIFE_UP;
                create = true;
            }
        } else {
            if (line.find('[') != string::npos){
                type = ASTEROID;
                line = line.substr(1);
            } else if (line.find(']') != string::npos){
                line = line.substr(0, line.size() - 1);
            } else if (line.find('{') != string::npos){
                line = line.substr(1);
            } else if (line.find('}') != string::npos){
                line = line.substr(0, line.size() - 1);
            }
            full_cell_count += read_celestial_shape(celestial_shape, line);
        }

        if (create){
            CelestialObject* celestial = new CelestialObject(celestial_shape, type, row, time_of_appearance);
            celestial->full_cell_count = full_cell_count;
            celestial->occupied_cells = full_cell_count;
            if (celestial_objects_list_head == nullptr){
                celestial_objects_list_head = celestial;
            } else {
                CelestialObject* current = celestial_objects_list_head;
                while(current->next_celestial_object != nullptr){
                    current = current->next_celestial_object;
                }
                current->next_celestial_object = celestial;
            }
            full_cell_count = 0;
            create = false;
            celestial_shape.clear();
            type = NONE;
        }
    }
}


// Print the entire space grid
void AsteroidDash::print_space_grid() const {
    cout << "Tick: " << game_time << endl;
    cout << "Lives: " << player->lives << endl;
    cout << "Ammo: " << player->current_ammo << endl;
    cout << "Score: " << current_score << endl;
    if (leaderboard.head_leaderboard_entry != nullptr) {
        cout << "High Score: " << ((current_score < leaderboard.head_leaderboard_entry->score) ?
                                    leaderboard.head_leaderboard_entry->score : current_score) << endl;
    } else {
        cout << "High Score: " << current_score << endl;
    }
    
    if (game_over) {
        cout << "Player: " << player->player_name << endl;
    }

    if (space_grid.empty()) {
        cout << "Empty space grid" << endl;
        return;
    }

    for (int i = 0; i < space_grid.size(); i++){
        for (int j = 0; j < space_grid[i].size(); j++){
            if (space_grid[i][j]) {
                cout << occupiedCellChar << " ";
            } else {
                cout << unoccupiedCellChar << " ";
            }
        }
        cout << endl;
    }
}


// Function to update the space grid with player, celestial objects, and any other changes
// It is called in every game tick before moving on to the next tick.
void AsteroidDash::update_space_grid() {
    //Clear the space grid
    for (int i = 0; i < space_grid.size(); i++){
        for (int j = 0; j < space_grid[i].size(); j++){
            space_grid[i][j] = 0;
        }
    }

    move_bullets();

    //Update celestial objects
    CelestialObject* current = celestial_objects_list_head;
    while (current != nullptr){
        CelestialObject* next = current->next_celestial_object;
        if (update_celestial_pos(current)){
            move_celestial(current);
        }
        current = next;
    }

    //Delete bullets that are out of bounds or have hit an asteroid
    delete_bullet();

    //Draw bullets
    for (Bullet* bullet : bullets){
        if (bullet->row >= 0 && bullet->col >=0 && bullet->row < space_grid.size() && bullet->col < space_grid[0].size()){
            space_grid[bullet->row][bullet->col] = 1;
        }
    }

    //Draw the player's spacecraft if the game is not over
    if (!game_over){
        for (int i = player->position_row; i < player->position_row + player->spacecraft_shape.size(); i++){
            for (int j = player->position_col; j < player->position_col + player->spacecraft_shape[0].size(); j++){
                if (i >= 0 && i < space_grid.size() && j >= 0 && j < space_grid[i].size()){
                    if (player->spacecraft_shape[i - player->position_row][j - player->position_col]){
                        space_grid[i][j] = 1;
                    }
                }
            }
        }
    }
}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot() {
    if (player->current_ammo > 0){
        player->current_ammo--;
        int row = (2 * player->position_row + player->spacecraft_shape.size()) / 2;
        int col = player->position_col + player->spacecraft_shape[0].size() -1;
        Bullet* bullet = new Bullet(row, col);
        bullets.push_back(bullet);
    }
}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash() {
    CelestialObject* current = celestial_objects_list_head;
    while(current != nullptr){
        CelestialObject* next = current->next_celestial_object;
        CelestialObject::delete_rotations(current);
        delete current;
        current = next;
    }
    delete player;
}


int AsteroidDash::read_celestial_shape(vector<vector<bool>> &celestial_shape, string line){
    int count = 0;
    vector<bool> row;
    for(char c : line){
        if(c == '1'){
            row.push_back(true);
            count++;
        } else {
            row.push_back(false);
        }
    }
    celestial_shape.push_back(row);
    return count;
}

bool AsteroidDash::handle_collision(CelestialObject* celestial, int row, int col, int celestial_row, int celestial_col){
    if (celestial == nullptr){
        return false;
    }

    for (Bullet* bullet : bullets){
        if (bullet->row == row && bullet->col == col && celestial->object_type == ASTEROID){
            bullet->is_hit = true;
            current_score += 10;
            celestial->occupied_cells--;
            if (celestial->occupied_cells == 0){
                current_score += celestial->full_cell_count * 100;
                delete_celestial(celestial);
                return true;
            }
            celestial->update_celestial(celestial_row, celestial_col);
            celestial->connect_rotations();
            CelestialObject* current = celestial_objects_list_head;
            CelestialObject* prev = nullptr;
            while (current != nullptr && current != celestial){
                prev = current;
                current = current->next_celestial_object;
            }
            if (prev != nullptr){
                // previous celestial object is connected to updated celestial object
                if (celestial->right_rotation != nullptr) {
                    CelestialObject* right = prev->right_rotation;
                    while (right != nullptr && right != current){
                        right->next_celestial_object = celestial->right_rotation;
                        right = right->right_rotation;
                    }
                    prev->next_celestial_object = celestial->right_rotation;
                } else {
                    CelestialObject* right = prev->right_rotation;
                    while (right != nullptr && right != current){
                        right->next_celestial_object = celestial;
                        right = right->right_rotation;
                    }
                    prev->next_celestial_object = celestial;
                }
            }
            return false;
        }
    }

    //cout << "row:" << row << " " << "col:" << col << " " << "player row:" << player->position_row << " " << "player last row:" << player->position_row + player->spacecraft_shape.size() -1 << " " << "player col:" << player->position_col << " " << "player last col:" << player->position_col + player->spacecraft_shape[0].size() -1 << endl; // Debugging
    if (row >= player->position_row && row <= player->position_row + player->spacecraft_shape.size() -1 && col >= player->position_col && col <= player->position_col + player->spacecraft_shape[0].size() -1 && player->spacecraft_shape[row - player->position_row][col - player->position_col]){
        if (celestial->object_type == AMMO){
            if (player->current_ammo < player->max_ammo){
                player->current_ammo++;
            }
        } else if (celestial->object_type == LIFE_UP){
            player->lives++;
        } else if (celestial->object_type == ASTEROID){
            player->lives--;
            if (player->lives == 0){
                game_over = true;
            }
        }
        delete_celestial(celestial);
        return true;
    }

    return false;
}

bool AsteroidDash::update_celestial_pos(CelestialObject* celestial){
    int col = game_time - celestial->time_of_appearance;
    if (col < 0 || celestial == nullptr){
        return false;
    }
    if (col >= 0){
        for (int i = celestial->starting_row; i < celestial->starting_row + celestial->shape.size() -1; i++){
            for (int j = space_grid[0].size()-1-col; j < space_grid[0].size()-1-col+celestial->shape[0].size()-1; j++){
                if (i>=0 && i < space_grid.size() && j >= 0 && j < space_grid[i].size()){
                    //cout << "i:" << i << "j:" << j << endl; // Debugging
                    if (celestial->shape[i - celestial->starting_row][j - (space_grid[0].size()-1-col)] && handle_collision(celestial, i, j, i - celestial->starting_row, j - (space_grid[0].size()-1-col))){
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void AsteroidDash::move_celestial(CelestialObject* celestial){
    int col = game_time - celestial->time_of_appearance;
    int next_col;
    if (col < 0 || celestial == nullptr){
        return;
    }
    if (col >= 0){
        for (int i = celestial->starting_row; i < celestial->starting_row + celestial->shape.size(); i++){
            for (int j = space_grid[0].size()-1-col; j < space_grid[0].size()-1-col+celestial->shape[0].size(); j++){
                if (i>=0 && i < space_grid.size() && j >= 0 && j < space_grid[i].size()){
                    if (celestial->shape[i - celestial->starting_row][j - (space_grid[0].size()-1-col)]){
                        space_grid[i][j] = 1;
                    }
                }
            }
            next_col = space_grid[0].size()-1-col+celestial->shape[0].size();
        }
        if (next_col < 0){
            delete_celestial(celestial);
        }
    }
    
}

void AsteroidDash::delete_celestial(CelestialObject* celestial){
    if (celestial == nullptr){
        return;
    }
    CelestialObject* current = celestial_objects_list_head;
    CelestialObject* prev = nullptr;
    while (current != nullptr && current != celestial){
        prev = current;
        current = current->next_celestial_object;
    }
    if (current != nullptr){
        if (prev == nullptr){
            celestial_objects_list_head = current->next_celestial_object;
        } else {
            CelestialObject* right = prev->right_rotation;
            while (right != prev){
                right->next_celestial_object = current->next_celestial_object;
                right = right->right_rotation;
            }
            prev->next_celestial_object = current->next_celestial_object;
        }
        CelestialObject::delete_rotations(current);
        delete current;
    }
}

void AsteroidDash::delete_bullet(){
    for (int i = bullets.size()-1; i >= 0; i--){
        Bullet* bullet = bullets[i];
        if (bullet->is_hit || bullet->out_of_bound){
            bullets.erase(bullets.begin() + i);
            delete bullet;
        }
    }
}

int AsteroidDash::control_celestial(CelestialObject* celestial){
    int cell_count = 0;
    for (int i = 0; i < celestial->shape.size(); i++){
        for (int j = 0; j < celestial->shape[i].size(); j++){
            if (celestial->shape[i][j]){
                cell_count++;
            }
        }
    }
    return cell_count;
}

void AsteroidDash::move_bullets(){
    for (int i = 0; i < bullets.size(); i++){
        bullets[i]->move(space_grid[0].size()-1);
    }
}