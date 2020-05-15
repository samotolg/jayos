#ifndef INCLUDE_HW_H_
#define INCLUDE_HW_H_

#define SET_BIT(p, n) ((p) |=  (1 << (n)))
#define CLR_BIT(p, n) ((p) &= ~(1 << (n)))


// Generic Timer Memory Mapped Register
// (Refer to ARM Architecture Reference I2.3.1, I5.6, I5.7)
// The memory map of fast model is defined below.
// (Refer to Fast Model Reference Manual 7.9.3)
#define REG_CNTCR					((volatile unsigned int*)0x2a430000) // REFCLK_CNTControl
#define REG_CNTREAD					((volatile unsigned int*)0x2a800000) // REFCLK_CNTRead
#define REG_CNTCTL					(0x2a810000) // AP_REFCLK_CNTCTL
#define REG_CNTBASE0				(0x2a820000) // AP_REFCLK_CNTBase0
#define REG_CNTBASE1				(0x2a830000) // AP_REFCLK_CNTBase1

#define REG_CNT_FCREQ				(0x00<<3)		// Frequence Table
#define REG_CNT_ENABLE				(0x01<<0)		// Timer Enable
#define REG_CNT_HDBG				(0x01<<1)		// Halt-on-Debug
#define REG_CNT_SCEN				(0x01<<2)		// Scaling

// The CNTACR<n>, Counter Access Control Register, n=0 ~ 7
#define CNTACR_RPCT					(0x01<<0)	// Read-only access to CNTPCT
#define CNTACR_RVCT					(0x01<<1)	// Read-only access to CNTVCT
#define CNTACR_RFRQ					(0x01<<2)	// Read-only access to CNTFRQ
#define CNTACR_RVOFF				(0x01<<3)	// Read-only access to CNTVOFF
#define CNTACR_RWVT					(0x01<<4)	// Read/write access to the Virtual Timer register
#define CNTACR_RWPT					(0x01<<5)	// Read/write access to the EL1 Physical Timer

// The CNTCTLBase frame
#define CNTCTL_CNTFRQ				((volatile unsigned int*)(REG_CNTCTL + 0x000)) // Counter Frequency register [secure-only]
#define CNTCTL_CNTNSAR				((volatile unsigned int*)(REG_CNTCTL + 0x004)) // Counter Non-Secure Access register [secure-only]
#define CNTCTL_CNTTIDR				((volatile unsigned int*)(REG_CNTCTL + 0x008)) // Counter Timer ID register
#define CNTCTL_CNTACR(n)			((volatile unsigned int*)(REG_CNTCTL + 0x040 + 4*n)) // Counter Access Control register N
#define CNTCTL_CNTVOFF(n)			((volatile unsigned int*)(REG_CNTCTL + 0x080 + 8*n)) // Virtual Offset register N
#define CNTCTL_CounterID(n)			((volatile unsigned int*)(REG_CNTCTL + 0xFD0 + 4*n)) // CounterID register 0~11

// The CNTBaseN and CNTEL0BaseN frames
#define CNTBASE_CNTPCT(n)			((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x000))  // Physical Counter Register
#define CNTBASE_CNTVCT(n)			((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x008))  // Virtual Count Register
#define CNTBASE_CNTFRQ(n)			((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x010))  // Counter Frequency Register
#define CNTBASE_CNTEL0ACR(n)		((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x014))  // Counter EL0 Access Control Register
#define CNTBASE_CNTVOFF(n)			((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x018))  // Virtual Offset Register
#define CNTBASE_CNTP_CVAL(n)		((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x020))  // Pyysical Timer CompareValue Register
#define CNTBASE_CNTP_TVAL(n)		((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x028))  // Physical TimerValue Register
#define CNTBASE_CNTP_CTL(n)			((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x02C))  // Physical Timer Control Register
#define CNTBASE_CNTV_CVAL(n)		((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x030))  // Virtual Timer CompareValue Register
#define CNTBASE_CNTV_TVAL(n)		((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x038))  // Virtual TimerValue Register
#define CNTBASE_CNTV_CTL(n)			((volatile unsigned int*)(REG_CNTBASE0 + 0x10000*n + 0x03C))  // Virtual Timer Control Register

#define CNTP_CTL_ENABLE				(0x01<<0)
#define CNTP_CTL_IMASK				(0x01<<1)
#define CNTP_CTL_ISTATUS			(0x01<<2)


// FVP Software Controllable LED
#define FVP_SYS_SW					((volatile unsigned int*)(0x1c010004))
#define FVP_SYS_LED					((volatile unsigned int*)(0x1c010008))
#define FVP_USERSW_ON(n)			{ uint32_t l = *FVP_SYS_SW; l |= (0x01<<n); *FVP_SYS_SW = l; }
#define FVP_USERSW_OFF(n)			{ uint32_t l = *FVP_SYS_SW; l |= (0x01<<n); *FVP_SYS_SW = l; }
#define FVP_BOOTSW_ON(n)			{ uint32_t l = *FVP_SYS_SW; l |= (0x01<<(n+8)); *FVP_SYS_SW = l; }
#define FVP_BOOTSW_OFF(n)			{ uint32_t l = *FVP_SYS_SW; l |= (0x01<<(n+8)); *FVP_SYS_SW = l; }
#define FVP_LED_ON(n)				{ uint32_t l = *FVP_SYS_LED; l |= (0x01<<n); *FVP_SYS_LED = l; }
#define FVP_LED_OFF(n)				{ uint32_t l = *FVP_SYS_LED; l &= ~(0x01<<n); *FVP_SYS_LED = l; }


#endif /* INCLUDE_HW_H_ */



