#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <unistd.h>
#include <bits/stdc++.h>
#include "input.h"
#include "snake.h"
#include "snake_map.h"
#include <utility>
#include "macros.h"

using namespace std;

Snake snake;
SnakeMap snake_map(&snake);

void initialize()
{
    input_init();
    input_enter_off();
}

void start_game()
{
    while (true)
    {
        snake.update_movement();
        snake_map.wait_for_food_thread();
        snake.check_movement(snake_map.get_food());
        if (snake.is_dead())
        {
            cout << "GAME IS OVER" << endl;
            snake.terminate_input_thread();
            break;
        }
        snake_map.clear_map();
        snake_map.update_items_location();
        snake_map.redraw();

        usleep(PAUSE_LENGTH);

        snake.validate_direction();
    }
}

int main()
{
    initialize();
    start_game();
    return 0;
}
