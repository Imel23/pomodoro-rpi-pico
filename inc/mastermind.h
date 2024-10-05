#ifndef MASTERMIND_H
#define MASTERMIND_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "buttons_handler.h"
#include "st7735.h"
#include "fonts.h"

/*########################### MASTERMIND GAME CONSTANTS #################################*/

#define MAX_SEQUENCE_LENGTH 4
#define MAX_ATTEMPTS 10
#define NUM_COLORS 4

// Define color codes
#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3

// Screen dimensions and layout constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

/*########################### MASTERMIND GAME STATE #################################*/

typedef struct
{
    int secret_sequence[MAX_SEQUENCE_LENGTH];
    int guess_sequence[MAX_SEQUENCE_LENGTH];
    int current_guess_index;
    int attempts;
    int correct_color_and_position;
    int correct_color_wrong_position;
    bool game_over;
    bool waiting_to_start;
    bool first_run; // Add this line
} GameState;

/*########################### MASTERMIND GAME FUNCTIONS #################################*/

void generate_secret_sequence(void);
void handle_button_press(int color);
void provide_feedback(void);
void check_game_status(void);
void reset_game(void);
void mastermind_init_view(void);
void mastermind_game_display(void);
void mastermind_game_logic(void);
void display_guess(void);
void display_feedback(int correct_color_and_position, int correct_color_wrong_position, int remaining_attempts);
void display_victory_message(void);
void display_failure_message(void);

#endif // MASTERMIND_H
