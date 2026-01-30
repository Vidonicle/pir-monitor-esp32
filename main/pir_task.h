#ifndef PIR_TASK_H
#define PIR_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void pir_task_start(QueueHandle_t q);

#endif /* PIR_TASK_H */