#include <stdio.h>
#include "mastermind.h"
#include "state_machine.h"

/*########################### MASTERMIND GAME STATE #################################*/

GameState game_state;
uint8_t history_x[4] = {135, 140, 145, 150};
uint8_t history_y = 10;

/*########################### MASTERMIND GAME LOGIC #################################*/

void generate_secret_sequence(void)
{
    // Seed the random number generator if necessary
    // srand((unsigned int)time(NULL)); // Uncomment if time function is available
    srand(to_us_since_boot(get_absolute_time()));
    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        game_state.secret_sequence[i] = rand() % NUM_COLORS + 1;
    }
}

void handle_button_press(int color)
{
    if (game_state.current_guess_index < MAX_SEQUENCE_LENGTH && !game_state.game_over)
    {
        game_state.guess_sequence[game_state.current_guess_index++] = color;
        display_guess();
    }
}

void provide_feedback(void)
{
    int checked_secret[MAX_SEQUENCE_LENGTH] = {0};
    int checked_guess[MAX_SEQUENCE_LENGTH] = {0};

    game_state.correct_color_and_position = 0;
    game_state.correct_color_wrong_position = 0;

    // First pass: Check for correct color and position
    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        if (game_state.guess_sequence[i] == game_state.secret_sequence[i])
        {
            game_state.correct_color_and_position++;
            checked_secret[i] = checked_guess[i] = 1;
        }
    }

    // Second pass: Check for correct color in wrong position
    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        if (!checked_guess[i])
        {
            for (int j = 0; j < MAX_SEQUENCE_LENGTH; j++)
            {
                if (!checked_secret[j] && game_state.guess_sequence[i] == game_state.secret_sequence[j])
                {
                    game_state.correct_color_wrong_position++;
                    checked_secret[j] = 1;
                    break;
                }
            }
        }
    }
}

void check_game_status(void)
{
    if (game_state.correct_color_and_position == MAX_SEQUENCE_LENGTH)
    {
        display_victory_message();
        game_state.game_over = true;
    }
    else
    {
        game_state.attempts++;
        if (game_state.attempts >= MAX_ATTEMPTS)
        {
            display_failure_message();
            game_state.game_over = true;
        }
        else
        {
            display_feedback(game_state.correct_color_and_position, game_state.correct_color_wrong_position, MAX_ATTEMPTS - game_state.attempts);
            display_history();
            game_state.current_guess_index = 0; // Reset for the next guess
        }
    }
}

void reset_game(void)
{
    memset(&game_state, 0, sizeof(GameState));
    game_state.waiting_to_start = true;
    game_state.game_over = false;
    // Note: We do not reset 'first_run' here to preserve its value
}

void mastermind_game_logic(void)
{

    if (game_state.first_run)
    {
        // Waiting for the player to press 'START' to display instructions
        if (is_start_pause_pressed)
        {
            is_start_pause_pressed = false;
            game_state.first_run = false;
            mastermind_display_instructions(); // Display instructions and prompt to press START again
        }
    }
    else if (game_state.waiting_to_start)
    {
        // Waiting for the player to press 'START' to begin the game
        if (is_start_pause_pressed)
        {
            is_start_pause_pressed = false;
            game_state.waiting_to_start = false;
            mastermind_game_display(); // Display game screen
            generate_secret_sequence();
            display_feedback(game_state.correct_color_and_position, game_state.correct_color_wrong_position, MAX_ATTEMPTS - game_state.attempts);
        }
    }
    else if (!game_state.game_over)
    {
        // Game is in progress
        if (game_state.current_guess_index < MAX_SEQUENCE_LENGTH)
        {
            // Process color selection buttons
            if (is_settings_pressed)
            {
                is_settings_pressed = false;
                handle_button_press(GREEN);
            }
            if (is_increase_pressed)
            {
                is_increase_pressed = false;
                handle_button_press(RED);
            }
            if (is_decrease_pressed)
            {
                is_decrease_pressed = false;
                handle_button_press(BLUE);
            }
            if (is_start_pause_pressed)
            {
                is_start_pause_pressed = false;
                handle_button_press(YELLOW);
            }
        }
        else
        {
            // Guess is complete, wait for 'START' button to submit guess
            if (is_start_pause_pressed)
            {
                is_start_pause_pressed = false;
                provide_feedback();
                check_game_status();

                if (!game_state.game_over)
                {
                    clear_guess_display(); // Clear circles after submitting the guess
                }
            }
        }
    }
    else
    {
        interrupt_flag = 2;
        // Game over, wait for 'START' button to reset
        if (is_start_pause_pressed)
        {
            is_start_pause_pressed = false;
            reset_game();
            history_y = 10;
            game_state.first_run = true; // Reset to initial state
            mastermind_init_view();      // Display initial screen asking to press START
        }
    }
}

/*########################### MASTERMIND GAME UI #################################*/

void mastermind_init_view(void)
{
    reset_game();
    game_state.first_run = true; // Game just started

    ST7735_FillScreen(ST7735_BLACK);

    // Center "Mastermind" title
    const char *title = "Mastermind";
    int title_width = strlen(title) * 11; // Assuming Font_11x18 is 11 pixels wide
    int title_x = (WIDTH - title_width) / 2;
    ST7735_DrawString(title_x + 10, 30, title, Font_11x18, ST7735_CYAN, ST7735_BLACK);

    // Draw a decorative line under the title
    ST7735_DrawLineThick(22, 55, 130, 55, ST7735_MAGENTA, 2);

    // Center "Press START to begin" prompt
    const char *prompt = "Press START to begin";
    int prompt_width = strlen(prompt) * 6; // Assuming Font_7x10 is 6 pixels wide
    int prompt_x = (WIDTH - prompt_width) / 2;
    ST7735_DrawString(prompt_x + 5, 80, prompt, Font_7x10, ST7735_YELLOW, ST7735_BLACK);
    interrupt_flag = 1;
}

void mastermind_display_instructions(void)
{
    ST7735_FillScreen(ST7735_BLACK);

    // Display game instructions
    const char *instructions[] = {
        "Instructions:",
        "Guess the secret",
        "color code!",
        "4 colors, 10 tries",
        "Use buttons to select",
        "Press START to begin"};

    int start_y = 15;
    for (int i = 0; i < 6; i++)
    {
        int text_width = strlen(instructions[i]) * 6; // Assuming Font_7x10
        int text_x = (WIDTH - text_width) / 2;
        ST7735_DrawString(text_x, start_y + i * 15, instructions[i], Font_7x10, ST7735_WHITE, ST7735_BLACK);
    }
}

void mastermind_game_display(void)
{
    ST7735_FillScreen(ST7735_BLACK);

    // Draw 4 empty circles for color selection
    int circle_y = HEIGHT / 2 + 10; // Centered vertically
    int circle_radius = 10;
    int total_width = MAX_SEQUENCE_LENGTH * (2 * circle_radius) + (MAX_SEQUENCE_LENGTH - 1) * 10;
    int start_x = (WIDTH - total_width) / 2;

    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        int circle_x = start_x + i * (2 * circle_radius + 10);
        ST7735_DrawCircle(circle_x + circle_radius, circle_y, circle_radius, ST7735_WHITE);
    }

    // Draw feedback area at the bottom
    ST7735_DrawRect(10, HEIGHT - 30, WIDTH - 20, 20, ST7735_CYAN);
    const char *feedback_text = "Mastermind";
    int feedback_width = strlen(feedback_text) * 6; // Assuming Font_7x10
    int feedback_x = (WIDTH - feedback_width) / 2;
    ST7735_DrawString(feedback_x, 15, feedback_text, Font_7x10, ST7735_BLACK, ST7735_CYAN);

    ST7735_DrawLineThick(130, 0, 130, 128, ST7735_CYAN, 2);
}

void display_guess(void)
{
    int circle_y = HEIGHT / 2 + 10; // Centered vertically
    int circle_radius = 10;
    int total_width = MAX_SEQUENCE_LENGTH * (2 * circle_radius) + (MAX_SEQUENCE_LENGTH - 1) * 10;
    int start_x = (WIDTH - total_width) / 2;

    for (int i = 0; i < game_state.current_guess_index; i++)
    {
        int circle_x = start_x + i * (2 * circle_radius + 10);
        uint16_t color;
        switch (game_state.guess_sequence[i])
        {
        case RED:
            color = ST7735_RED;
            break;
        case GREEN:
            color = ST7735_GREEN;
            break;
        case BLUE:
            color = ST7735_BLUE;
            break;
        case YELLOW:
            color = ST7735_YELLOW;
            break;
        default:
            color = ST7735_WHITE;
            break;
        }
        ST7735_FillCircle(circle_x + circle_radius, circle_y, circle_radius, color);
    }
}

void display_history(void)
{

    for (int i = 0; i < game_state.current_guess_index; i++)
    {
        uint16_t color;
        switch (game_state.guess_sequence[i])
        {
        case RED:
            color = ST7735_RED;
            break;
        case GREEN:
            color = ST7735_GREEN;
            break;
        case BLUE:
            color = ST7735_BLUE;
            break;
        case YELLOW:
            color = ST7735_YELLOW;
            break;
        default:
            color = ST7735_WHITE;
            break;
        }
        ST7735_DrawFastVLine(history_x[i], history_y, 5, color);
    }
    history_y += 10;
}

void clear_guess_display(void)
{
    int circle_y = HEIGHT / 2 + 10; // Centered vertically
    int circle_radius = 10;
    int total_width = MAX_SEQUENCE_LENGTH * (2 * circle_radius) + (MAX_SEQUENCE_LENGTH - 1) * 10;
    int start_x = (WIDTH - total_width) / 2;

    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        int circle_x = start_x + i * (2 * circle_radius + 10);

        // Clear the filled circle by drawing over it with the background color
        ST7735_FillCircle(circle_x + circle_radius, circle_y, circle_radius, ST7735_BLACK);

        // Redraw the empty circle outline
        ST7735_DrawCircle(circle_x + circle_radius, circle_y, circle_radius, ST7735_WHITE);
    }
}
void display_feedback(int correct_position, int correct_color, int remaining_attempts)
{
    char feedback[30];
    char remaining_attempts_str[20];
    snprintf(feedback, sizeof(feedback), "Position: %d", correct_position);
    ST7735_FillRectangle(5, 30, WIDTH - 30, 20, ST7735_BLACK); // Clear previous feedback
    ST7735_DrawString(5, 30, feedback, Font_7x10, ST7735_WHITE, ST7735_BLACK);

    snprintf(feedback, sizeof(feedback), "Color: %d", correct_color);
    ST7735_FillRectangle(5, 45, WIDTH - 30, 20, ST7735_BLACK); // Clear previous feedback
    ST7735_DrawString(5, 45, feedback, Font_7x10, ST7735_WHITE, ST7735_BLACK);

    snprintf(remaining_attempts_str, sizeof(remaining_attempts_str), "Tries: %d", remaining_attempts);
    int text_width = strlen(remaining_attempts_str) * 8;
    ST7735_FillRectangle(5, 60, text_width, 10, ST7735_BLACK); // Clear only the necessary area
    ST7735_DrawString(5, 60, remaining_attempts_str, Font_7x10, ST7735_RED, ST7735_BLACK);
}

void display_victory_message(void)
{
    ST7735_FillScreen(ST7735_BLACK);
    const char *message = "You Win!";
    int message_width = strlen(message) * 11; // Assuming Font_11x18
    int message_x = (WIDTH - message_width) / 2;
    ST7735_DrawString(message_x, HEIGHT / 2 - 9, message, Font_11x18, ST7735_GREEN, ST7735_BLACK); // Adjusted to center vertically
}

void display_failure_message(void)
{
    ST7735_FillScreen(ST7735_BLACK);
    const char *message = "Game Over";
    int message_width = strlen(message) * 11; // Assuming Font_11x18
    int message_x = (WIDTH - message_width) / 2;
    ST7735_DrawString(message_x, HEIGHT / 2 - 50, message, Font_11x18, ST7735_RED, ST7735_BLACK);

    // Display the secret sequence
    const char *secret_msg = "Secret:";
    int secret_msg_width = strlen(secret_msg) * 6;
    int secret_msg_x = (WIDTH - secret_msg_width) / 2;
    ST7735_DrawString(secret_msg_x, HEIGHT / 2 - 20, secret_msg, Font_7x10, ST7735_WHITE, ST7735_BLACK); // Adjusted vertical position

    int circle_y = HEIGHT / 2 + 10;
    int circle_radius = 10;
    int total_width = MAX_SEQUENCE_LENGTH * (2 * circle_radius) + (MAX_SEQUENCE_LENGTH - 1) * 10;
    int start_x = (WIDTH - total_width) / 2;

    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        int circle_x = start_x + i * (2 * circle_radius + 10);
        uint16_t color;
        switch (game_state.secret_sequence[i])
        {
        case RED:
            color = ST7735_RED;
            break;
        case GREEN:
            color = ST7735_GREEN;
            break;
        case BLUE:
            color = ST7735_BLUE;
            break;
        case YELLOW:
            color = ST7735_YELLOW;
            break;
        default:
            color = ST7735_WHITE;
            break;
        }
        ST7735_FillCircle(circle_x + circle_radius, circle_y, circle_radius, color);
    }
}