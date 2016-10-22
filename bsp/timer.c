#include "bsp.h"
#include "reg_macro.h"
#include "artik_board_config.h"
#include "irqs.h"
#include "gic.h"

#define MCT_CH_OFFSET		0x100

#define MCT_TCNTCB(ch)		(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x00)
#define MCT_TCON(ch)		(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x20)
#define MCT_INT_CSTAT(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x30)
#define MCT_INT_ENB(ch)		(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x34)
#define MCT_WSTAT(ch)		(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x40)

unsigned long tick_count;

void timer_handler(int irq_num)
{
	int ch = (irq_num - IRQ_MCT0_L0_IRQ);
	writel(1, MCT_INT_CSTAT(ch));

	tick_count++;
	// tick control
}

void timer_init(int ch)
{
	
}

void timer_enable(int ch, u32 time, void (*timer_cb)(int))
{
	writel(time, MCT_TCNTCB(ch));

	// clear any asserted interrupt
	writel(1, MCT_INT_CSTAT(ch));

	// 
	writel(1, MCT_INT_ENB(ch));
	writel(3, MCT_TCON(ch));

	intr_setup_isr(ch + IRQ_MCT0_L0_IRQ, timer_cb);
	intr_enable(ch + IRQ_MCT0_L0_IRQ);
}

void timer_disable(int ch)
{
	writel(0, MCT_INT_ENB(ch));
	writel(0, MCT_TCON(ch));

	intr_setup_isr(ch + IRQ_MCT0_L0_IRQ, NULL);
	intr_disable(ch + IRQ_MCT0_L0_IRQ);
}


unsigned long get_tick(void)
{
	return tick_count;
}





