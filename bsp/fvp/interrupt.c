#include "stdio.h"
#include "rtos_type.h"
#include "hw.h"
#include "aarch64.h"
#include "interrupt.h"
#include "hal_interrupt.h"

extern volatile GicCput_t* GicCpu;
extern volatile GicDist_t* GicDist;

static InterHdlr_fptr sHandlers[INTERRUPT_HANDLER_NUM];

void hal_interrupt_init()
{
	uint32_t pending_irq;

	/* GICD init */
	// Enable distributor
	GicDist->distributorctrl.bits.Enable = GIC_DIST_CTLR_ENABLE;
	
	/* GICC init */
	// Disable CPU interface
	GicCpu->cpucontrol.bits.Enable = GIC_CPU_CTLR_DISABLE;

	GicCpu->prioritymask.bits.Prioritymask = GIC_CPU_PMR_PRIO_MIN;

	// Clear all of the active interrupts
	uint32_t interrupt_id = GicCpu->interruptack.bits.InterruptID;
	for(pending_irq = ( interrupt_id & GIC_CPU_IAR_INTR_IDMASK ); 
		( pending_irq != GIC_CPU_IAR_SPURIOUS_INTR );
		pending_irq = ( interrupt_id & GIC_CPU_IAR_INTR_IDMASK ) )
		GicCpu->endofinterrupt.bits.InterruptID = interrupt_id;

	// Enable CPU interface
	GicCpu->cpucontrol.bits.Enable = GIC_CPU_CTLR_ENABLE;

	enable_irq();

	putstr("Enable IRQ, DAIF = ");
	uint32_t val = raw_read_daif();
	puthex(val);
    putstr("\n");
	
}

void hal_interrupt_enable(uint32_t interrupt_num)
{
	uint32_t regoff = (interrupt_num / GIC_DIST_ISENABLER_PER_REG);
	uint32_t bitoff = (interrupt_num % GIC_DIST_ISENABLER_PER_REG);

	SET_BIT(GicDist->setenable[regoff], bitoff);
}

void hal_interrupt_disable(uint32_t interrupt_num)
{
	uint32_t regoff = (interrupt_num / GIC_DIST_ICENABLER_PER_REG);
	uint32_t bitoff = (interrupt_num % GIC_DIST_ICENABLER_PER_REG);

	CLR_BIT(GicDist->setenable[regoff], bitoff);
	SET_BIT(GicDist->clearenable[regoff], bitoff);
}

void hal_interrupt_register_handler(InterHdlr_fptr handler, uint32_t interrupt_num)
{
	sHandlers[interrupt_num] = handler;
}

void hal_interrupt_run_handler(void)
{
	uint32_t interrupt_num = GicCpu->interruptack.bits.InterruptID;

	if (sHandlers[interrupt_num] != NULL)
	{
		sHandlers[interrupt_num]();
	}

	GicCpu->endofinterrupt.bits.InterruptID = interrupt_num;
}

