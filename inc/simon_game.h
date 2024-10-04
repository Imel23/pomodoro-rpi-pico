#ifndef SIMON_GAME_H
#define SIMON_GAME_H

typedef enum
{
    IDLE_S,
    PLAYSEQ_S,
    RIGHTSEQ_S,
    WRONGSEQ_S
} state_e;

extern state_e simon_state;
void simon_game();

#endif // SIMON_GAME_H