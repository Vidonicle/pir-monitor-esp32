#ifndef PIR_TASK_H
#define PIR_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t event_queue;

void pir_gpio_init(void);
void pir_task(void *pv);

#endif /* PIR_TASK_H */