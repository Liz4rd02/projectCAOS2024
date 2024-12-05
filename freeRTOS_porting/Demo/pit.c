#include "clock.h"
#include "pit.h"
#include "lpuart.h"
#include "FreeRTOS.h"
#define PIT_FREQUENCY 5UL

void NVIC_EnableIRQ(void) {
	NVIC_ISER3 |= (0x01 << 0);	//works 
	
}

void NVIC_DisableIRQ(void) {
	NVIC_ICER3 |= (0x01 << 0);
}
/*
void NVIC_SetPriority(uint8_t priority) {
	NVIC_IPR24 |= (priority << 0);
}
*/


void NVIC_SetPriority(uint8_t priority) {
    // Ensure priority is within the valid range (4 bits)
    priority &= 0xF;
    NVIC_IPR24 = (NVIC_IPR24 & ~(0xFF)) | (priority << 4); // Upper 4 bits are used for priority
}

void NVIC_ClearPendingIRQ(void) {
    // Clear pending interrupt for IRQ 96
	NVIC_ICPR3 |= (0x01 << 0);
}

void pit_clear_interrupt(void) {
	PIT_TFLG0 = 0x01; // Write 1 to clear the interrupt flag                            
}



// Initialize the PIT module
void pit_init() {
	pit_disable();
	PIT_LDVAL0 = ( configCPU_CLOCK_HZ / PIT_FREQUENCY ) + 1UL; //The starting point where the timer begins to reduce.
    	PIT_TCTRL0 = PIT_TCTRL_TEN | PIT_TCTRL_TIE ; //To the Timer Interupt Enable Flag (TIE) and enable the interrupt
	PIT_MCR = 0x00;           // Enable PIT module (disable MDIS)

	NVIC_SetPriority(5);
	NVIC_EnableIRQ(); 
}


void pit_disable(void) {
	PIT_MCR = 0x02;  //set the MDIS bit to 1, to disable PIT
    	PIT_TCTRL0 |= (0x00 << 0U); //Disable the interrupt	
}


