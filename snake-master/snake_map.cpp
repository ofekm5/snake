#include "snake_map.h"
#include <pthread.h>
#include <iostream>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <time.h>
#include "macros.h"

using namespace std;

SnakeMap::SnakeMap(Snake *snake)
{
    this->snake = snake;
    this->clear_map();
    srand(time(NULL));
    update_snake_food();
}

void SnakeMap::update_items_location(){
    this->update_snake_location();
    this->update_food_location();
}

void SnakeMap::update_snake_location(){
    vector<pair<int, int>> snake_parts = snake->snake_parts;
    for (int i = 0; i < snake_parts.size(); i++)
    {
        pair<int, int> tmp = snake_parts[i];
        this->map_array[tmp.first][tmp.second] = SNAKE_CHAR;
    }

    this->update_snake_head();
}

void SnakeMap::update_food_location(){
    if (snake->food_eaten){
        this->update_snake_food();
    }
    this->map_array[snake_food.first][snake_food.second] = SNAKE_FOOD_CHAR;
}

pair<int, int> SnakeMap::get_food(){
    return this->snake_food;
}

void SnakeMap::redraw(void)
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            cout << "\033[33m" << this->map_array[i][j] << "\033[0m" << " ";
        }
        cout << endl;
    }

    draw_score(); 
}

void SnakeMap::update_snake_food()
{
    while (true){
        int random_i = rand() % MAP_WIDTH;
        int random_j = rand() % MAP_HEIGHT;
        if (this->map_array[random_i][random_j] == MAP_CHAR){
            this->snake_food = make_pair(random_i, random_j);
            snake->food_eaten = false;
            break;
        }
    }
}

void SnakeMap::clear_map()
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            this->map_array[i][j] = MAP_CHAR;
        }
    }

    for (int i = 0; i < MAP_END; i++)
    {
        cout << endl;
    }
}

void SnakeMap::update_snake_head()
{
    char snake_head_char = SNAKE_CHAR;
    enum Direction direction = this->snake->get_direction();
    switch (direction)
    {
        case West:
            snake_head_char = SNAKE_HEAD_WEST;
            break;
        case North:
            snake_head_char = SNAKE_HEAD_NORTH;
            break;
        case East:
            snake_head_char = SNAKE_HEAD_EAST;
            break;
        case South:
            snake_head_char = SNAKE_HEAD_SOUTH;
            break;
    }
    pair<int, int> snake_head = this->snake->snake_head;
    this->map_array[snake_head.first][snake_head.second] = snake_head_char;
}

void SnakeMap::draw_score(void)
{
    cout << "Score:" << snake->length << endl;
}