#ifndef FLASH_H
#define FLASH_H

#include "state_machine.h"

void flash_write(time_s *time);
void flash_read(time_s *time);

#endif // SIMON_GAME_H