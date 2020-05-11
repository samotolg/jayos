#ifndef LIB_AARCH64_H_
#define LIB_AARCH64_H_ 

/* CurrentEL, Current Exception Level */
#define CURRENT_EL_MASK		0x3
#define CURRENT_EL_SHIFT	2

/* DAIF, Interrupt Mask Bits */
#define DAIF_DBG_BIT		(1<<3)	/* Debug mask bit */
#define DAIF_ABT_BIT		(1<<2)	/* Asynchronous abort mask bit */
#define DAIF_IRQ_BIT		(1<<1)	/* IRQ mask bit */
#define DAIF_FIQ_BIT		(1<<0)	/* FIQ mask bit */

/* Interrupt flags */
#define AARCH64_DAIF_FIQ	(1)		/* FIQ */
#define AARCH64_DAIF_IRQ	(2)		/* IRQ */

/* Timer */  
#define CNTV_CTL_ENABLE		(1 << 0)	/* Enables the timer */	
#define CNTV_CTL_IMASK		(1 << 1)	/* Timer interrupt mask bit */
#define CNTV_CTL_ISTATUS	(1 << 2)	/* The status of the timer interrupt. This bit is read-only */

/* Wait For Interrupt */
#define wfi()		asm volatile("wfi" : : : "memory")

/* AArch64 exception types*/
/* Current EL with SP0 */
#define AARCH64_EXC_SYNC_SP0      (0x1)   /* Synchronous */
#define AARCH64_EXC_IRQ_SP0       (0x2)   /* IRQ/vIRQ */
#define AARCH64_EXC_FIQ_SP0       (0x3)   /* FIQ/vFIQ */
#define AARCH64_EXC_SERR_SP0      (0x4)   /* SError/vSError */
/* Current EL with SPx */
#define AARCH64_EXC_SYNC_SPX      (0x11)
#define AARCH64_EXC_IRQ_SPX       (0x12)
#define AARCH64_EXC_FIQ_SPX       (0x13)
#define AARCH64_EXC_SERR_SPX      (0x14)
/* Lower EL using AArch64 */
#define AARCH64_EXC_SYNC_AARCH64  (0x21)
#define AARCH64_EXC_IRQ_AARCH64   (0x22)
#define AARCH64_EXC_FIQ_AARCH64   (0x23)
#define AARCH64_EXC_SERR_AARCH64  (0x24)
/* Lower EL using AArch32 */
#define AARCH64_EXC_SYNC_AARCH32  (0x31)
#define AARCH64_EXC_IRQ_AARCH32   (0x32)
#define AARCH64_EXC_FIQ_AARCH32   (0x33)
#define AARCH64_EXC_SERR_AARCH32  (0x34)

/* exception_frame offset definitions */
#define EXC_FRAME_SIZE (288)	/* sizeof(struct _exception_frame) */
#define EXC_EXC_TYPE_OFFSET (0)	/* __asm_offsetof(struct _exception_frame, exc_type) */
#define EXC_EXC_ESR_OFFSET (8)	/* __asm_offsetof(struct _exception_frame, exc_esr) */
#define EXC_EXC_SP_OFFSET (16)	/* __asm_offsetof(struct _exception_frame, exc_sp) */
#define EXC_EXC_ELR_OFFSET (24)	/* __asm_offsetof(struct _exception_frame, exc_elr) */
#define EXC_EXC_SPSR_OFFSET (32)/* __asm_offsetof(struct _exception_frame, exc_spsr) */

/* PSTATE and special purpose register access functions */
uint32_t raw_read_current_el(void);
uint32_t get_current_el(void);
uint32_t raw_read_daif(void);
void raw_write_daif(uint32_t daif);
void enable_debug_exceptions(void);
void enable_serror_exceptions(void);
void enable_irq(void);
void enable_fiq(void);
void disable_debug_exceptions(void);
void disable_serror_exceptions(void);
void disable_irq(void);
void disable_fiq(void);

/* SPSR_EL1, Saved Program Status Register (EL1) */
uint32_t raw_read_spsr_el1(void);
void raw_write_spsr_el1(uint32_t spsr_el1);

/* ISR_EL1, Interrupt Status Register */
uint32_t raw_read_isr_el1(void);
uint64_t raw_read_rvbar_el1(void);
void raw_write_rvbar_el1(uint64_t rvbar_el1);
uint64_t raw_read_vbar_el1(void);
void raw_write_vbar_el1(uint64_t vbar_el1);

#endif  /* LIB_AARCH64_H_ */