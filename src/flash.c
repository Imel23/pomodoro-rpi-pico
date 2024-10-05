#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "state_machine.h"

// Define the size of a flash sector (4KB)
#define FLASH_SECTOR_SIZE1 4096

// Define where to store the data in flash (at the end of the flash)
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - 2 * FLASH_SECTOR_SIZE1)
#define MAGIC_NUMBER 0xA5A5A5A5 // Example magic number

// Structure to store multiple variables
typedef struct
{
    uint32_t magic; // Magic number for validation
    uint8_t sessions_f;
    uint8_t workDuration_f;
    uint8_t shortBreak_f;
    uint8_t longBreak_f;
} TaskData;

// Allocate a buffer for the structure (aligned with the flash sector size)
uint8_t flash_buffer[FLASH_SECTOR_SIZE1] __attribute__((aligned(4))) = {0};

// Function to write multiple variables to flash
void flash_write_multiple(TaskData *data)
{

    // Disable interrupts
    uint32_t ints = save_and_disable_interrupts();

    // Uncomment flash erase and write operations later
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE1);

    memcpy((void *)flash_buffer, data, sizeof(TaskData));
    flash_range_program(FLASH_TARGET_OFFSET, flash_buffer, sizeof(TaskData));

    // Re-enable interrupts
    restore_interrupts(ints);
}

// Function to read multiple variables from flash
void flash_read_multiple(TaskData *data)
{
    // Read the data from flash memory
    const uint8_t *flash_data = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

    // Copy the data into the TaskData structure
    memcpy(data, flash_data, sizeof(TaskData));
    // Check if the magic number is valid
    if (data->magic != MAGIC_NUMBER)
    {
        // The flash contains uninitialized or invalid data; initialize with defaults
        printf("Flash data invalid or uninitialized, setting defaults.\n");

        data->magic = MAGIC_NUMBER; // Set the magic number for next write
        data->sessions_f = 8;
        data->shortBreak_f = 5;
        data->longBreak_f = 15;
        data->workDuration_f = 25;

        // After initializing data, write it to flash
        flash_write_multiple(data);
    }
    else
    {
        printf("Flash data read successfully.\n");
    }
}

void flash_write(time_s *time)
{
    TaskData data = {MAGIC_NUMBER, time->sessions, time->workDuration, time->shortBreak, time->longBreak};
    // Save the data to flash
    flash_write_multiple(&data);
}

void flash_read(time_s *time)
{
    TaskData loaded_data = {0}; // Initialize a structure to hold the read data
    flash_read_multiple(&loaded_data);
    time->sessions = loaded_data.sessions_f;
    time->workDuration = loaded_data.workDuration_f;
    time->longBreak = loaded_data.longBreak_f;
    time->shortBreak = loaded_data.shortBreak_f;
}