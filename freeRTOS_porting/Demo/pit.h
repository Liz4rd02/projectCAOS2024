#include <stdint.h>
#ifndef PIT_H
#define PIT_H

// Base address for the PIT peripheral
#define PIT_BASE_ADDR       0x400B0000  // Updated base address for S32K358

// Offsets for PIT registers
#define PIT_MCR             (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x00))  /* PIT Control Register */
#define PIT_LDVAL0          (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x100)) /* Timer Load Value Register (Channel 0) */
#define PIT_CVAL0           (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x104)) /* Current Timer Value Register (Channel 0) */
#define PIT_TCTRL0          (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x108)) /* Timer Control Register for Channel 0 */
#define PIT_TFLG0           (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x10C)) /* Timer Flag Register for Channel 0 */

// Timer Control Flags
#define PIT_TCTRL_TEN       (1 << 0)    /* Timer Enable */
#define PIT_TCTRL_TIE       (1 << 1)    /* Timer Interrupt Enable */

#define PIT_IRQ 96

void pit_init(void);

void pit_clear_interrupt(void);

void PIT_Handler(void);

#endif /* PIT_H */

