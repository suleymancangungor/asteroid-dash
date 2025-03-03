#include "CelestialObject.h"


// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
        : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance) {
    // TODO: Your code here
}


// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
        : shape(other->shape),  // Copy the 2D vector shape
          object_type(other->object_type),  // Copy the object type
          starting_row(other->starting_row),  // Copy the starting row
          time_of_appearance(other->time_of_appearance),  // Copy the time of appearance
          next_celestial_object(other->next_celestial_object)  // Copy the next celestial object
{
    // TODO: Your code here
}

// Function to delete rotations of a given celestial object. It should free the dynamically allocated
// memory for each rotation.
void CelestialObject::delete_rotations(CelestialObject *target) {
    if (target == nullptr) {
        return;
    }
    CelestialObject* current = target->right_rotation;
    while (current != nullptr && current->shape != target->shape) {
        CelestialObject* next = current->right_rotation;
        delete current;
        current = next;
    }
}


void CelestialObject::create_rotations(){
    CelestialObject* current = this;
    while(true){
        vector<vector<bool>> rotated_shape = rotate_shape(current->shape);
        if (rotated_shape == this->shape){
            break;
        }
        CelestialObject* rotated = new CelestialObject(rotated_shape, this->object_type, this->starting_row, this->time_of_appearance);
        current->right_rotation = rotated;
        rotated->left_rotation = current;
        current = rotated;
    }
    current->right_rotation = this;
    this->left_rotation = current;
}


vector<vector<bool>> CelestialObject::rotate_shape(vector<vector<bool>> shape){
    vector<vector<bool>> rotated_shape;
    for (int i = 0; i < shape[0].size(); i++){
        vector<bool> row;
        for (int j = shape.size() - 1; j >= 0; j--){
            row.push_back(shape[j][i]);
        }
        rotated_shape.push_back(row);
    }
    return rotated_shape;
}


void CelestialObject::update_celestial(int i, int j){
    shape[i][j] = false;
    delete_rotations(this);
    create_rotations();
}


void CelestialObject::connect_rotations(){
    if (this->next_celestial_object != nullptr){
        CelestialObject* right = this->right_rotation;
        while (right != nullptr && right != this){
            right->next_celestial_object = this->next_celestial_object;
            right = right->right_rotation;
        }
    }
}