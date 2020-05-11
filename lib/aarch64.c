/*
 * Reference: ARM® Architecture Reference Manual ARMv8, for ARMv8-A architecture profile
 */
#include <stdint.h>
#include "aarch64.h"

/* CurrentEL, Current Exception Level
	EL, bits [3:2]
		Current exception level. Possible values of this field are:
		00 EL0
		01 EL1
		10 EL2
		11 EL3
*/
uint32_t raw_read_current_el(void)
{
	uint32_t current_el;

	__asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");
	return current_el;
}

uint32_t get_current_el(void)
{
	uint32_t current_el = raw_read_current_el();
	return ((current_el >> CURRENT_EL_SHIFT) & CURRENT_EL_MASK);
}

/* DAIF, Interrupt Mask Bits
	Allows access to the interrupt mask bits.

	D, bit [9]: Debug exceptions.
	A, bit [8]: SError (System Error) mask bit.
	I, bit [7]: IRQ mask bit.
	F, bit [6]: FIQ mask bit.
	value: 
		0 Exception not masked.
		1 Exception masked.
*/
uint32_t raw_read_daif(void)
{
	uint32_t daif;

	__asm__ __volatile__("mrs %0, DAIF\n\t" : "=r" (daif) :  : "memory");
	return daif;
}

void raw_write_daif(uint32_t daif)
{
	__asm__ __volatile__("msr DAIF, %0\n\t" : : "r" (daif) : "memory");
}

void enable_debug_exceptions(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_DBG_BIT)  : "memory");
}

void enable_serror_exceptions(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_ABT_BIT)  : "memory");
}

void enable_irq(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_IRQ_BIT)  : "memory");
}

void enable_fiq(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_FIQ_BIT)  : "memory");
}

void disable_debug_exceptions(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_DBG_BIT)  : "memory");
}

void disable_serror_exceptions(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_ABT_BIT)  : "memory");
}

void disable_irq(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_IRQ_BIT)  : "memory");
}

void disable_fiq(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_FIQ_BIT)  : "memory");
}

/* SPSR_EL1, Saved Program Status Register (EL1)
	Holds the saved processor state when an exception is taken to EL1.
*/
uint32_t raw_read_spsr_el1(void)
{
	uint32_t spsr_el1;

	__asm__ __volatile__("mrs %0, SPSR_EL1\n\t" : "=r" (spsr_el1) :  : "memory");
	return spsr_el1;
}

void raw_write_spsr_el1(uint32_t spsr_el1)
{
	__asm__ __volatile__("msr SPSR_EL1, %0\n\t" : : "r" (spsr_el1) : "memory");
}


/* 
ISR_EL1, Interrupt Status Register
	Shows whether an IRQ, FIQ, or SError interrupt is pending.
*/
uint32_t raw_read_isr_el1(void)
{
	uint32_t isr_el1;

	__asm__ __volatile__("mrs %0, ISR_EL1\n\t" : "=r" (isr_el1) :  : "memory");
	return isr_el1;
}

/* 
RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented)
	If EL1 is the highest exception level implemented, contains the 
	IMPLEMENTATION DEFINED address that execution starts from after reset when
	executing in AArch64 state.
*/
uint64_t raw_read_rvbar_el1(void)
{
	uint64_t rvbar_el1;

	__asm__ __volatile__("mrs %0, RVBAR_EL1\n\t" : "=r" (rvbar_el1) :  : "memory");
	return rvbar_el1;
}

void raw_write_rvbar_el1(uint64_t rvbar_el1)
{
	__asm__ __volatile__("msr RVBAR_EL1, %0\n\t" : : "r" (rvbar_el1) : "memory");
}

/* VBAR_EL1, Vector Base Address Register (EL1)
	Holds the exception base address for any exception that is taken to EL1.
*/
uint64_t raw_read_vbar_el1(void)
{
	uint64_t vbar_el1;

	__asm__ __volatile__("mrs %0, VBAR_EL1\n\t" : "=r" (vbar_el1) :  : "memory");
	return vbar_el1;
}

void raw_write_vbar_el1(uint64_t vbar_el1)
{
	__asm__ __volatile__("msr VBAR_EL1, %0\n\t" : : "r" (vbar_el1) : "memory");
}
