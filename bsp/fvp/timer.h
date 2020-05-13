#ifndef HAL_QEMU_TIMER_H_
#define HAL_QEMU_TIMER_H_

#define TIMER_WAIT		1	/* Assert Timer IRQ after n secs */

static uint32_t cntfrq;		/* System frequency */

//#define TIMER_INTERRUPT          27
#define TIMER_INTERRUPT          34


/* CNTV_CTL_EL0, Counter-timer Virtual Timer Control register */
uint32_t raw_read_cntv_ctl(void);
void disable_cntv(void);
void enable_cntv(void);

/* CNTFRQ_EL0, Counter-timer Frequency register */
uint32_t raw_read_cntfrq_el0(void);
void raw_write_cntfrq_el0(uint32_t cntfrq_el0);

/* CNTVCT_EL0, Counter-timer Virtual Count register */
uint64_t raw_read_cntvct_el0(void);

/* CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register */
uint64_t raw_read_cntv_cval_el0(void);
void raw_write_cntv_cval_el0(uint64_t cntv_cval_el0);

#endif /* HAL_QEMU_TIMER_H_ */
