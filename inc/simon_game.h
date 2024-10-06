#ifndef SIMON_GAME_H
#define SIMON_GAME_H

typedef enum
{
    IDLE_S,
    PLAYSEQ_S,
    RIGHTSEQ_S,
    WRONGSEQ_S,
    WIN
} state_e;

extern state_e simon_state;
extern bool idle_first_time;
void simon_game();

#endif // SIMON_GAME_H