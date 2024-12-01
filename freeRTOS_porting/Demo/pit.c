#include "clock.h"
#include "pit.h"
//#include "core_cm7.h"
#include "lpuart.h"
//#include "ARMCM7.h"


// Initialize the PIT module
void pit_init(void) {
    	PIT_MCR = 0x00;           // Enable PIT module (disable MDIS)
    	PIT_LDVAL0 = 0xF42400;    // Load value for a 1-second interval (assuming a 1 MHz clock)
    	PIT_TCTRL0 = PIT_TCTRL_TEN | PIT_TCTRL_TIE; // Enable Timer 0 and enable its interrupt
    
    
	//CMSDK_TIMER1->RELOAD   = ( configCPU_CLOCK_HZ / tmrTIMER_1_FREQUENCY ) + 1UL;

	/* It takes two parameters: the interrupt number and the priority level to assign.*/
	//NVIC_SetPriority( PIT_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY );
}

// PIT handler called by the ISR
void PIT_Handler(void) {
    	pit_clear_interrupt();  // Clear the interrupt flag by writing '1'
    	my_printf("PIT Interrupt triggered!\n"); // Print message when PIT interrupt occurs
    	/*
    	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    	vTaskNotifyGiveFromISR(xTaskHandle, &xHigherPriorityTaskWoken);
    	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    	*/
}



void NVIC_EnableIRQ(void) {
	NVIC_ISER3 |= (0x01 << 0);
	
}

void NVIC_DisableIRQ(void) {
	NVIC_ICER3 |= (0x01 << 0);
}

void NVIC_SetPriority(uint8_t priority) {
	NVIC_IPR24 |= (priority << 0);
}

/*
 *Pit initialization, for example we want to generate an interrupt every 10ms
 *Formula to calculate the initial value: Start_value = (timer_period/clock_period)-1
*/

void pit_init(uint8_t divid) {
	// enable the clock for the PIT module and the prescaler
	clock_config();
	set_prescaler(divid);
	MC_ME_PRTN0_COFB1_CLKEN |= (0x01 << 12U);
		
	pit_disable();
	PIT_LDVAL0 = 0x0007A11FU;  //The starting point where the timer begins to reduce.
	
    	PIT_TCTRL0 = PIT_TCTRL_TIE | PIT_TCTRL_TIE ; //To the Timer Interupt Enable Flag (TIE) and enable the interrupt
	
	//add abilitation of channel bit (CHN) in case we want to use more PITs toghether
	PIT_MCR &= ~(0x01 << 1U); // set the MDIS bit to 0, to enable PIT
	NVIC_SetPriority(5);
	NVIC_EnableIRQ(); 
}


void pit_disable(void) {
	PIT_MCR |= (0X01 << 1U); //set the MDIS bit to 1, to disable PIT
    	PIT_TCTRL0 |= (0x00 << 0U); //Disable the interrupt	
}

void pit_clear_interrupt(void) {
	PIT_TFLG0 = 0x01; // Write 1 to clear the interrupt flag                            
}


/*
PIT Registers:

    PIT_MCR, PIT_TCTRL0, PIT_TFLG0, and PIT_LDVAL0 are memory-mapped registers for the PIT timer.
    The Load Value Register (PIT_LDVAL0) is set to trigger an interrupt every 10 millisecond (assuming a 50 MHz timer clock, adjust this as per your configuration).
    The Timer Control Register (PIT_TCTRL0) is configured to enable the timer and enable interrupt generation when the timer counts down to zero.

Interrupt Handler:

PIT_ISR() is a simple interrupt handler that clears the interrupt flag (PIT_TFLG0) and sends a message through LPUART each time the PIT triggers.
The interrupt is triggered when the PIT reaches the load value set in PIT_LDVAL0*/
