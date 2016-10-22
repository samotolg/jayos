#include "gic.h"
#include "fos_timer.h"

/*
* Description:
* Interrupt Distributor memory mapped registers
*/
static interrupt_distributor * const id = (interrupt_distributor *)GICD_BASE;

/*
* Description:
* CPU Interface memory mapped registers - aliased for each CPU
*/
static cpu_interface * const ci = (cpu_interface *)GICC_BASE;

/*
* Description:
* Array of IRQ handlers
*/
static void (*task_irq_handler[NO_OF_INTERRUPTS_IMPLEMENTED])(u32)  = { 0, };
 //void (* BE_Top_ISR_Table[BE_INT_LEVEL_MAX])(void) = {0, };

static void default_irq_handler(u32 irq)
{

}
 
/***** See corresponding header file for description *****/
unsigned long intr_distributor_init(void)
{
	u32 i;

	/* Check the ARM Legacy Identification Register Field values */
	if ((id->peri_id[0] !=0x90)||(id->peri_id[1]!=0xB4)||(id->peri_id[2]!=0x2B))
	{
		return FALSE;
	}

	/* Disable the whole controller */
	id->control = 0x00000000;

	/* Disable all interrupts */
	for (i = 0; i < NO_OF_INTERRUPTS_IMPLEMENTED >> 5; i++)		// /32
	{
		id->enable.clear[i] = 0xFFFFFFFF;
	}

	/* Clear all interrupts */
	for (i = 1; i < NO_OF_INTERRUPTS_IMPLEMENTED >> 5; i++)		// /32
	{
		id->pending.clear[i] = 0xFFFFFFFF;
	}

	/* Reset interrupt priorities */
	for (i = 8; i < NO_OF_INTERRUPTS_IMPLEMENTED >> 2; i++)		// /4
	{
		id->priority[i] = 0x00000000;
	}

	/* Reset interrupt targets */
	for (i = 0; i < NO_OF_INTERRUPTS_IMPLEMENTED >> 2; i++)		// /4
	{
		id->target[i] = 0x00000000;
	}

	/* Set interrupt configuration (level high sensitive, 1-N) */
	for (i = 2; i < NO_OF_INTERRUPTS_IMPLEMENTED >> 4; i++)		// /16
	{
		id->configuration[i] = 0x55555555;
	}

	/* Install the IRQ handler */
	//intr_install_handler();

	/* Finally, enable the interrupt controller */
	id->control = 0x00000001;

#if defined(IRQ_COUNT_SAVE)
	 for (i = 0; i < NO_OF_INTERRUPTS_IMPLEMENTED; i++)
	 {
		 irq_count[i] = 0;
	 }
#endif

	return TRUE;
}




/**
* @fn     void intr_fiq_handler(void)
* @brief  fiq interrupt handler function
* @param  none 
* @return none
* @see
*/
void intr_fiq_handler(void)
{

	// TO.DO

}


extern timer_cb_func_type 	os_timer_cb[2];

/*
* Description:
* Interrupt handler
*
* Remarks:
* Re-entrant interrupt handler
*
* Inputs:
* ack_register : Interrupt ID as read from the ACK register
* 			   by the low level interrupt handler
*
* Return Value:
* None
* 
*/
void intr_irq_handler(void)
{
#if 0
//	u32 source;
	u32 ack_register, interrupt, raw_interrupt, old_priority_mask;

	ack_register = (ci->interrupt_ack & 0x3FF);

	/*	NOTE: If the priority mask or the interrupt priority has changed between 
		when the interrupt was asserted and when we ackowledge it, making it of 
		a lower (higher than the mask) priority, its value will be read as 
		SPURIOUS_INTERRUPT */

	/* Get the highest priority interrupt */
	raw_interrupt = ack_register;

	/* Retrieve interrupt source CPU and interrupt ID */
//	source = raw_interrupt >> CPUID_SHIFT;
	interrupt = raw_interrupt & INTERRUPT_MASK;

	/*	Inhibit interrupts of lower priority than the current.
		Interrupts of lower priority will now remain pending in the distributor
		but will be masked out by the interface - until we return from the raw handler.
		Note that interrupts are disabled at this stage */
	old_priority_mask = ci->priority_mask;
	ci->priority_mask = (ci->running_priority & PRIORITY_MASK_MASK);

	if (interrupt == SPURIOUS_INTERRUPT)
	{
		return;
	}

	/*	To avoid pre-emption and rescheduling (which could cause migration), if we are
		currently handling the scheduler system tick (or an external interrupt) we
		do not re-enable interrupts */
#if 0 //#if defined(CONFIG_CORE_D5_CA7MP1)||defined(CONFIG_CORE_D5_CA9MP)
	if (interrupt != SCHEDULER_IPI && interrupt != GIC1_CHAIN_ID)
		EnableIRQ();
#endif

	/*	Some peripherals require the interrupt to be cleared at source.
		This must be done in the handler */
	if (task_irq_handler[interrupt])
		task_irq_handler[interrupt](interrupt);
	else
		default_irq_handler(interrupt);


	/* Clear the interrupt */
	ci->end_of_interrupt = raw_interrupt;

	/* Restore original priority mask */
	ci->priority_mask = old_priority_mask;   
	
#if 0 //#if defined(CONFIG_CORE_D5_CA7MP1)||defined(CONFIG_CORE_D5_CA9MP)
	DisableIRQ();
#endif
#else
	if (*(volatile unsigned int *)TIMER_MIS(0)) {
		*(volatile unsigned int *)TIMER_INTCLR(0) = 1;
		os_timer_cb[0](1);
	}

	if (*(volatile unsigned int *)TIMER_MIS(1)) {
		*(volatile unsigned int *)TIMER_INTCLR(1) = 1;
		os_timer_cb[1](1);
	}
#endif
}


#if 0
/***** See corresponding header file for description *****/
u32 intr_priority_set(u32 int_id, u32 priority)
{
	u32 word, bit_shift, temp, old_priority;

	priority &= 0xF;

	/* There are 4 interrupt priority registers per word */
	word = int_id >> 2; 							/* Register where this interrupt priority is */
	bit_shift = (int_id & 0x3) * 8 + 4;			/* Offset of this interrupt's priority within the register */

	temp = old_priority = id->priority[word];	/* Get priority register */
	temp &= ~((u32)0xF << bit_shift);			/* Reset the priority for this interrupt to 0 */
	temp |= (priority << bit_shift);			/* Set the new priority */

	id->priority[word] = temp;					/* Write new priority */

	return ((old_priority >> bit_shift) & 0xF);	/* Return original priority */
}


/***** See corresponding header file for description *****/
u32 intr_priority_get(u32 int_id)
{
	u32 bit_shift, word;

	/* There are 4 interrupt priority registers per word */
	word = int_id >> 2; 
	bit_shift = (int_id & 0x3) * 8 + 4;

	return ((id->priority[word] >> bit_shift) & 0xF);
}
#endif


/***** See corresponding header file for description *****/
void intr_setup_isr(u32 int_id, irqHandler handler)
{
	if (int_id > NO_OF_INTERRUPTS_IMPLEMENTED) 
	{
		return;
	}
	task_irq_handler[int_id] = (void *)handler;
}


/***** See corresponding header file for description *****/
void intr_target_set(u32 int_id, u32 cpu, u32 set)
{
	u32 word, bit_shift, temp;

	/* There are 4 interrupt target registers per word */
	word = int_id >> 2;
	bit_shift = (int_id & 0x3) * 8;
	cpu = (1 << cpu) << bit_shift;

	temp = id->target[word];
	if (set)
	{
		temp |= cpu;
	}
	else
	{
		temp &= ~cpu;
	}
	id->target[word] = temp;
}


/***** See corresponding header file for description *****/
u32 intr_target_get(u32 int_id)
{
	u32 word, bit_shift;

	/* There are 4 interrupt target registers per word */
	word = int_id >> 2;
	bit_shift = (int_id & 0x3) * 8;

	return ((id->target[word] >> bit_shift) & 0xF);
}


/* interrupt distrubutor pending bit clear */
void intr_clear(u32 int_id)
{
	u32 word;

	word = int_id >> 5; 		// /32
	int_id = int_id & 0x1f; 	// %32
	int_id = 1 << int_id;

	id->pending.clear[word] = int_id;

}


/***** See corresponding header file for description *****/
void intr_enable(u32 int_id)
{
	u32 word;

	word = int_id >> 5;
	int_id = int_id & 0x1f;
	int_id = 1 << int_id;

	id->enable.set[word] = int_id;
}

void intr_disable(u32 int_id)
{
	u32 word;

	word = int_id >> 5;
	int_id = int_id & 0x1f;
	int_id = 1 << int_id;

	id->enable.clear[word] = int_id;
}

void intr_disable_all(void)
{
	u32 word;
	u32 i;

	word = NO_OF_INTERRUPTS_IMPLEMENTED >> 5;		// divide 32

	for (i = 0; i < word; i++)
	{
		id->enable.clear[word] = 0xFFFFFFFF;
	}
}


/***** See corresponding header file for description *****/
void intr_interface_init(void)
{
	u32 i;

	/* Clear up the bits of the distributor which are actually CPU-specific */
	id->pending.clear[0] = 0xFFFFFFFF;
	for (i = 0; i < 8; i++)
	{
		id->priority[i] = 0x00000000;
	}
	id->configuration[0] = 0xAAAAAAAA;
	id->configuration[1] = 0xAAAAAAAA;

	/* Disable the CPU Interface */
	ci->control = 0x00000000;

	/* Allow interrupts with higher priority (i.e. lower number) than 0xF */
	ci->priority_mask = 0x000000F0;

	/* All priority bits are compared for pre-emption */
	ci->binary_point = 0x00000003;

	/* Clear any pending interrupts */
	do
	{
		u32 temp;

		temp = ci->interrupt_ack;

		if ((temp & INTERRUPT_MASK) == SPURIOUS_INTERRUPT)
		{
			break;
		}

		ci->end_of_interrupt = temp;
	} while (TRUE);

	/* Enable the CPU Interface */
	ci->control = 0x00000001;
}


#if 0
/***** See corresponding header file for description *****/
u32 intr_running_priority(void)
{
	return ((ci->running_priority >> PRIORITY_SHIFT) & 0xF);
}


/***** See corresponding header file for description *****/
u32 intr_highest_pending_ID_get(void)
{
	return (ci->highest_pending & INTERRUPT_MASK);
}


/***** See corresponding header file for description *****/
u32 intr_highest_pending_source_get(void)
{
	return (ci->highest_pending & INTERRUPT_MASK);
}


/***** See corresponding header file for description *****/
u32 intr_binary_point_set(u32 binary_point)
{
	u32 temp, old_binary_point;

	temp = old_binary_point = ci->binary_point;
	temp &= ~BINARY_POINT_MASK; /* Clear current binary point value */

	ci->binary_point = temp | (binary_point & BINARY_POINT_MASK);

	return (old_binary_point & BINARY_POINT_MASK);
}


/***** See corresponding header file for description *****/
u32 intr_binary_point_get(void)
{
	return (ci->binary_point & BINARY_POINT_MASK);
}


/***** See corresponding header file for description *****/
u32 intr_priority_mask_set(u32 priority_mask)
{
	u32 temp, old_priority_mask;

	priority_mask &= PRIORITY_MASK_MASK; 

	temp = old_priority_mask = ci->priority_mask; /* Get priority mask - aliased */
	temp &= ~PRIORITY_MASK_MASK; /* Clear current priority mask */

	ci->priority_mask = temp | (priority_mask << PRIORITY_SHIFT); /* Set new priority mask */

	return ((old_priority_mask & PRIORITY_MASK_MASK) >> PRIORITY_SHIFT);
}


/***** See corresponding header file for description *****/
u32 intr_priority_mask_get(void)
{
	return ((ci->priority_mask & PRIORITY_MASK_MASK) >> PRIORITY_SHIFT);
}
#endif

/***** See corresponding header file for description *****/
bool gic_enabled(void)
{
	return (id->control & 0x01);
}

/* software generate interrupt */
void intr_sgi_send(u32 interrupt)
{
	u32 target_cpu = 0x1;	//[7:0] -> D5 : CA7/CA9 Single Core
	interrupt &= IPI_MASK;

	id->software_int = (target_cpu << IPI_TARGET_SHIFT) | interrupt;

}




