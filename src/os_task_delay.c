/**
 * @file os_task_delay.c
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_task.h"

void os_task_delay(const os_delta_ticks_t delay_ticks)
{
    vTaskDelay(delay_ticks);
}