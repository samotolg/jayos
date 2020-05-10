#ifndef __BSP_H__
#define __BSP_H__

#include "rtos_type.h"
#include "hw.h"


/**
 * Initialize the Interrupt Distributor
 *
 * Only one CPU should call this routine
 *
 * \retval FALSE The distributor is not present and/or correct
 * \retval TRUE  Initialization successful
 * 
 */
unsigned long intr_distributor_init(void);

/**
 * Initialize CPU Interface
 *
 * There is one CPU Interface per CPU, therefore all CPUs must call this routine
 *
 * \return No return value
 * 
 */
void intr_interface_init(void);

/**
 * Enable interrupts
 *
 * Use in place of *_irq() intrinsics if it is necessary to
 * keep track of the previous interrupt state
 *
 * \return The previous value of the I bit (previous interrupt state)
 * 
 */
void EnableIRQ(void);


/**
 * Disable interrupts
 *
 * Use in place of *_irq() intrinsics if it is necessary to
 * keep track of the previous interrupt state
 *
 * \return The previous value of the I bit (previous interrupt state)
 * 
 */
void DisableIRQ(void);


void uart_init(void);
void printf(const char *fmt,...);

void timer_init(int ch);

void timer_enable(int ch, u32 time, void (*timer_cb)(int));


unsigned long get_tick(void);


#endif	/* __BSP_H__ */
