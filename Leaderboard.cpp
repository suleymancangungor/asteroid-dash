#include "Leaderboard.h"

// Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
// variable will point to the highest all-times score, and all other scores will be reachable from it
// via the "next_leaderboard_entry" member variable pointer.
void Leaderboard::read_from_file(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(-1);
    }
    string line;
    while(getline(file, line)){
        if (line.empty()){
            continue;
        }
        istringstream iss(line);
        string order;
        unsigned long score;
        time_t timestamp;
        string name;
        iss >> score >> timestamp >> name;
        LeaderboardEntry* leaderboard_entry = new LeaderboardEntry(score, timestamp, name);
        insert(leaderboard_entry);
    }
}


// Write the latest leaderboard status to the given file in the format specified in the PA instructions
void Leaderboard::write_to_file(const string &filename) {
    ofstream file(filename);
    if (!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(-1);
    }
    LeaderboardEntry* current = head_leaderboard_entry;
    while (current != nullptr){
        file << current->score << " " << current->last_played << " " << current->player_name << endl;
        current = current->next;
    }
}

// Print the current leaderboard status to the standard output in the format specified in the PA instructions
void Leaderboard::print_leaderboard() {
    cout << "Leaderboard" << endl;
    cout << "--------------------------------" << endl;
    LeaderboardEntry* current = head_leaderboard_entry;
    int order = 1;
    while(current != nullptr){
        cout << order << ". " << current->player_name << " - " << current->score << " - " << put_time(localtime(&current->last_played), "%c %Z") << endl;
        current = current->next;
        order++;
    }
}

//  Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
//  is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
//  top 10 all-time high-scores should be kept in descending order by the score).
void Leaderboard::insert(LeaderboardEntry *new_entry) {
    //cout << "Inserting " << new_entry->player_name << " with score " << new_entry->score << endl; // Debugging
    if (head_leaderboard_entry == nullptr){
        head_leaderboard_entry = new_entry;
        return;
    }
    LeaderboardEntry* current = head_leaderboard_entry;
    LeaderboardEntry* prev = nullptr;
    while(current != nullptr && current->score >= new_entry->score){
        prev = current;
        current = current->next;
    }
    if (prev == nullptr){
        new_entry->next = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        prev->next = new_entry;
        new_entry->next = current;
    }
    //cout << "Inserted " << new_entry->player_name << " with score " << new_entry->score << endl; // Debugging

    // Remove the last entry if the leaderboard size exceeds 10
    LeaderboardEntry* last = head_leaderboard_entry;
    int count = 1;
    while (last != nullptr && count < MAX_LEADERBOARD_SIZE){
        last = last->next;
        count++;
    }
    if (count == MAX_LEADERBOARD_SIZE && last != nullptr){
        LeaderboardEntry* delete_entry = last->next;
        last->next = nullptr;
        delete delete_entry;
    }
    //cout << "Leaderboard size: " << count << endl; // Debugging
}

// Free dynamically allocated memory used for storing leaderboard entries
Leaderboard::~Leaderboard() {
    LeaderboardEntry* current = head_leaderboard_entry;
    while (current != nullptr) {
        LeaderboardEntry* next = current->next;
        delete current;
        current = next;
    }
    head_leaderboard_entry = nullptr;
}
