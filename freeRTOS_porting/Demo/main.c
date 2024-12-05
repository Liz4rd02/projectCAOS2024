#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpuart.h"
#include "pit.h"

#include <stdio.h>

#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )


void vAssertCalled(void) {
    taskDISABLE_INTERRUPTS();  // Disable all interrupts
    while (1) {
        // Optional: Add a debugging mechanism (e.g., blink LED, print a message via UART)
    }
}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    while (1) {
        // Optional: Add debugging mechanism
    }
}

// PIT handler called by the ISR
void PIT_Handler(void) {
	PIT_TFLG0 = 0x01;  // Clear interrupt flag
     	my_printf("PIT interrupt triggered!\n");
  
}


void vTask1(void *pvParameters) {
	(void)pvParameters;
    	for (;;) {
        	my_printf("Task 1 running\n");
        	vTaskDelay(pdMS_TO_TICKS(1000));  // 1 second
    	}
}

void vTask2(void *pvParameters) {
	(void)pvParameters;
   	for (;;) {
        	my_printf("Task 2 running\n");
        	vTaskDelay(pdMS_TO_TICKS(500));  // 500 ms
    	}
}

int main(void) {
    	// Initialize hardware (UART, PIT, etc.)
    	__asm("CPSIE I");  // Enable interrupts in ARM architecture
    	lpuart_init();
    	pit_init();

    	// Create tasks
    	//xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    	//xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    	// Start the scheduler
    	//vTaskStartScheduler();

    	// Should never reach here
    	for (;;);
    	return 0;
}

