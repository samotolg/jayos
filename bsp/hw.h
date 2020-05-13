#ifndef INCLUDE_HW_H_
#define INCLUDE_HW_H_

#define SET_BIT(p, n) ((p) |=  (1 << (n)))
#define CLR_BIT(p, n) ((p) &= ~(1 << (n)))


// Generic Timer Memory Mapped Register
// (Refer to ARM Architecture Reference I5.6)
// The memory map of fast model is defined below.
// (Refer to Fast Model Reference Manual 7.9.3)
#define REG_CNTCR					((volatile unsigned int*)0x2a430000) // REFCLK_CNTControl
#define REG_CNTREAD					((volatile unsigned int*)0x2a800000) // REFCLK_CNTRead
#define REG_CNTCTL					((volatile unsigned int*)0x2a810000) // AP_REFCLK_CNTCTL
#define REG_CNTBASE0				((volatile unsigned int*)0x2a820000) // AP_REFCLK_CNTBase0
#define REG_CNTBASE1				((volatile unsigned int*)0x2a830000) // AP_REFCLK_CNTBase1

#define CNTPCT(n)					(REG_CNTBASE##n + 0x000)  // Physical Counter Register
#define CNTVCT(n)					(REG_CNTBASE##n + 0x008)  // Virtual Count Register
#define CNTFRQ(n)					(REG_CNTBASE##n + 0x010)  // Counter Frequency Register
#define CNTEL0ACR(n)				(REG_CNTBASE##n + 0x014)  // Counter EL0 Access Control Register
#define CNTVOFF(n)					(REG_CNTBASE##n + 0x018)  // Virtual Offset Register
#define CNTP_CVAL(n)				(REG_CNTBASE##n + 0x020)  // Pyysical Timer CompareValue Register
#define CNTP_TVAL(n)				(REG_CNTBASE##n + 0x028)  // Physical TimerValue Register
#define CNTP_CTL(n)					(REG_CNTBASE##n + 0x02C)  // Physical Timer Control Register
#define CNTV_CVAL(n)				(REG_CNTBASE##n + 0x030)  // Virtual Timer CompareValue Register
#define CNTV_TVAL(n)				(REG_CNTBASE##n + 0x038)  // Virtual TimerValue Register
#define CNTV_TVAL(n)				(REG_CNTBASE##n + 0x038)  // Virtual TimerValue Register
#define CNTV_CTL(n)					(REG_CNTBASE##n + 0x03C)  // Virtual Timer Control Register





#define REG_CNT_FCREQ				(0x00<<3)		// Frequence Table
#define REG_CNT_ENABLE				(0x01<<0)		// Timer Enable
#define REG_CNT_HDBG				(0x01<<1)		// Halt-on-Debug
#define REG_CNT_SCEN				(0x01<<2)		// Scaling



#endif /* INCLUDE_HW_H_ */

