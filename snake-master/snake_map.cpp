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
    sem_init(&next_food_sema, 0, 1);
    schedule_next_food_task();
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

void SnakeMap::wait_for_food_thread(){
    pthread_join(food_thread, NULL);
}

char SnakeMap::get_map_array_item(int i, int j){
    return this->map_array[i][j];
}


void SnakeMap::schedule_next_food_task(){
    FoodThreadArgs* args = new FoodThreadArgs{this, this->get_food()};
    pthread_create(&food_thread, NULL, food_thread_task, args);
}

void* food_thread_task(void *arg)
{
    struct FoodThreadArgs *args = (struct FoodThreadArgs *)arg;
    SnakeMap* snake_map = args->map;
    pair<int,int> prev_snake_food = args->curr_food;
    pair<int,int> next_snake_food;

    while (true){
        int random_i = rand() % MAP_WIDTH;
        int random_j = rand() % MAP_HEIGHT;
        if (snake_map->get_map_array_item(random_i, random_j) == MAP_CHAR){
            next_snake_food = make_pair(random_i, random_j);
            snake_map->set_next_snake_food(make_pair(random_i, random_j));
            break;
        }
    }
    return NULL;
    //find_path_to_food(next_snake_food, prev_snake_food);
}

void SnakeMap::set_next_snake_food(pair<int,int> i_pair){
    sem_wait(&this->next_food_sema);
    this->next_snake_food = i_pair;
    sem_post(&this->next_food_sema);
}

// void find_path_to_food(pair<int,int> next_snake_food, pair<int,int> prev_snake_food){
//     vector<pair<int, int>> shortest_path;

//     //this->map_array[i][j] = SNAKE_FOOD_PATH;

//     return shortest_path;
// }

void SnakeMap::shift_snake_food(){
    sem_wait(&this->next_food_sema);
    this->snake_food = this->next_snake_food;
    sem_post(&this->next_food_sema);
}

void SnakeMap::update_food_location(){
    if (snake->food_eaten){
        snake->food_eaten = false;
        this->shift_snake_food();
        //embed_path_to_board();
        this->schedule_next_food_task();
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
            switch (this->map_array[i][j]) {
                case MAP_CHAR:
                    std::cout << "\033[33m" << this->map_array[i][j] << "\033[0m" << " "; // Yellow text
                    break;
                case SNAKE_FOOD_PATH:
                    std::cout << "\033[30;42m" << this->map_array[i][j] << "\033[0m" << std::endl; //green background
                    break;
                case SNAKE_FOOD_CHAR:
                    std::cout << "\033[31m" << this->map_array[i][j] << "\033[0m" << " "; //red text
                    break;
                default: // all snake parts
                    std::cout << "\033[34m" << this->map_array[i][j] << "\033[0m" << " "; //blue text
                    break;
            }

        }
        cout << endl;
    }

    draw_score(); 
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