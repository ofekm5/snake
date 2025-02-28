#ifndef _snake_map_h
#define _snake_map_h

#include "snake.h"
#include "macros.h"

void* food_thread_task(void *arg);

class SnakeMap
{
  public:
    SnakeMap(Snake *snake);
    void redraw();
    void update_items_location();
    void clear_map();
    pair<int, int> get_food();
    char get_map_array_item(int i, int j);
    void set_next_snake_food(pair<int,int> i_pair);
    void wait_for_food_thread();
  private:
  void schedule_next_food_task();
    char map_array[MAP_HEIGHT][MAP_WIDTH];
    pthread_t food_thread;
    void update_snake_food();
    Snake *snake;
    sem_t next_food_sema;
    void shift_snake_food();
    pair<int, int> snake_food;
    pair<int, int> next_snake_food;
    void draw_score();
    void update_snake_location();
    void update_food_location();
    void update_snake_head();
    void update_snake_food_and_path();
};

struct FoodThreadArgs {
  SnakeMap* map;
  std::pair<int, int> curr_food;
};

#endif