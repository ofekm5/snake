#ifndef _snake_map_h
#define _snake_map_h

#include "snake.h"
#include "macros.h"

class SnakeMap
{
  public:
    SnakeMap(Snake *snake);
    void redraw();
    void update_items_location();
    void clear_map();
    pair<int, int> get_food();
  private:
    char map_array[MAP_HEIGHT][MAP_WIDTH];
    void update_snake_food();
    Snake *snake;
    pair<int, int> snake_food;
    void draw_score();
    void update_snake_location();
    void update_food_location();
    void update_snake_head();
};

#endif