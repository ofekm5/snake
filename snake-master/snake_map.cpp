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
    stop_bfs = false;
    this->snake = snake;
    this->clear_map();
    srand(time(NULL));
    sem_init(&next_food_sema, 0, 1);
    schedule_next_food_task();
    presentingPathIterations = ITER_TO_SHOW_PATH;
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
    pthread_create(&food_thread, NULL, food_thread_task, this);
}

void* food_thread_task(void *arg)
{
    struct SnakeMap *snake_map = (struct SnakeMap *)arg;
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
}

void SnakeMap::set_next_snake_food(pair<int,int> i_pair){
    sem_wait(&this->next_food_sema);
    this->next_snake_food = i_pair;
    sem_post(&this->next_food_sema);
}

vector<pair<int,int>> SnakeMap::bfs(pair<int,int> next_snake_food, pair<int,int> prev_snake_food){
    std::vector<std::vector<bool>> visited(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));

    queue<pair<int, int>> q;
    map<pair<int, int>, pair<int, int>> parent;
    int options_x[] = {1, 0, -1, 0};
    int options_y[] = {0, 1, 0, -1};

    q.push(prev_snake_food);
    visited[prev_snake_food.first][prev_snake_food.second] = true;

    pair<int, int> food_position = make_pair(-1,-1);

    while(!q.empty()){
        pair<int, int> item = q.front();
        q.pop(); 

        if((next_snake_food.first == item.first) && (next_snake_food.second == item.second)){
            food_position = item;
            break;
        }

        for(int i=0;i<4;i++){
            int currx = item.first + options_x[i];
            int curry = item.second + options_y[i];

            if(currx >=0 && currx < MAP_WIDTH && curry >=0 && this->get_map_array_item(currx, curry)!=SNAKE_CHAR && !visited[currx][curry]){
                visited[currx][curry] = true;
                q.push(make_pair(currx, curry));
                parent[make_pair(currx, curry)] = {item.first, item.second};
            }
        }
    }

    if(food_position.first == -1){
        return {};
    }

    return form_path(food_position, prev_snake_food, parent);
}

vector<pair<int, int>> SnakeMap::form_path(pair<int, int> food_position, pair<int,int> prev_snake_food, map<pair<int, int>, pair<int, int>> parent){
    vector<pair<int, int>> path;
    pair<int, int> current = food_position;
    while(current != prev_snake_food){
        path.push_back(current);
        current = parent[current];
    }
    path.push_back(prev_snake_food); 
    reverse(path.begin(), path.end());

    return path;
}

void SnakeMap::shift_snake_food(){
    sem_wait(&this->next_food_sema);
    this->snake_food = this->next_snake_food;
    sem_post(&this->next_food_sema);
}

void SnakeMap::update_food_location(){
    if (snake->isFoodEaten()){
        snake->set_food_eaten(false);
        this->shift_snake_food();
        if(!stop_bfs){
            this->shortest_path = bfs(this->snake_food, this->snake->snake_head);
        }
        if(this->shortest_path.empty()){
            stop_bfs = true;
        }
        this->schedule_next_food_task();
        presentingPathIterations = ITER_TO_SHOW_PATH;
    }
    this->map_array[snake_food.first][snake_food.second] = SNAKE_FOOD_CHAR;
}

pair<int, int> SnakeMap::get_food(){
    return this->snake_food;
}

void SnakeMap::redraw(void)
{
    if(!stop_bfs && presentingPathIterations>0){
        for (const auto& item : shortest_path) {
            this->map_array[item.first][item.second] = SNAKE_FOOD_PATH;
        }
        presentingPathIterations--;
    }

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            switch (this->map_array[i][j]) {
                case MAP_CHAR:
                    std::cout << "\033[33m" << this->map_array[i][j] << "\033[0m" << " "; // Yellow text
                    break;
                case SNAKE_FOOD_PATH:
                    std::cout << "\033[30;42m" << MAP_CHAR << "\033[0m" << " "; //green background
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