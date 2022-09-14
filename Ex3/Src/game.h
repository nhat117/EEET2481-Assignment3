#ifndef GAME_INCLUDED
#define GAME_INCLUDED
void game_start();
void display_board_to_lcd();
void check_game_condition();
void shoot();
void resultBuzzer();
void reset_player_board();
void test_display_uart_board();
void handle_welcome();
void handle_load();
void handle_game();
void handle_end();
int message_type = 0;
int game_first_load = 0;
#endif