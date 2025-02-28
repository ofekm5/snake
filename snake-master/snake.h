#ifndef _snake_h
#define _snake_h

#include "input.h"
#include <thread>
#include <semaphore.h>
#include <vector>
#include <utility>
#include "macros.h"

using namespace std;

class Snake
{
  public:
    Snake();
    void update_direction(enum Direction direction);
    void update_next_direction(enum Direction direction);
    enum Direction get_direction();
    void validate_direction();
    vector<pair<int, int>> snake_parts;
    pair<int, int> snake_head;
    void update_movement();
    bool is_dead();
    bool isFoodEaten();
    void set_food_eaten(bool i_status);
    int length;
    void terminate_input_thread();
    void check_movement(pair<int, int> snake_food);
  private:
    bool dead;
    void drop_dead();
    bool food_eaten;
    pthread_t input_thread;
    sem_t snake_sema;
    enum Direction direction;
    enum Direction next_direction;
    int snake_world_array[MAP_HEIGHT][MAP_WIDTH];
    void clear_snake_world();
    void initialize_snake();
};

#endif