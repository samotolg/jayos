#ifndef __REG_MACRO_H__
#define __REG_MACRO_H__

/**
 *	 arm assembley function define
 */
#define nop() 	__asm__ __volatile__ ("mov\tr0,r0\t@ nop\n\t")
#define	sev()	__asm__ __volatile__ ("sev" : : : "memory")
#define	wfe()	__asm__ __volatile__ ("wfe" : : : "memory")
#define	wfi()	__asm__ __volatile__ ("wfi" : : : "memory")
//#define	isb()	__asm__ __volatile__ ("isb" : : : "memory")
#define	dmb()	__asm__ __volatile__ ("dmb" : : : "memory")


#define isb()		asm("isb" : : : "memory")
#define dsb()		asm("dsb" : : : "memory")
#define mb()		do { dsb();  outer_sync();} while (0)
#define rmb()		dsb()
#define __raw_readl(c)			(*(volatile unsigned int *)(c))
#define __raw_readw(c)			(*(volatile unsigned short *)(c))
#define __raw_readb(c)			(*(volatile unsigned char *)(c))
#define __raw_writel(v,c)		(*(volatile unsigned int *)(c) = (v))
#define __raw_writew(v,c)		(*(volatile unsigned short *)(c) = (v))
#define __raw_writeb(v,c)		(*(volatile unsigned char *)(c) = (v))
#define readl_relaxed(c)		({ unsigned int __v = __raw_readl(c); __v; })
#define readw_relaxed(c)		({ unsigned short __v = __raw_readw(c); __v; })
#define readb_relaxed(c)		({ unsigned char  __v = __raw_readb(c); __v; })
#define writel_relaxed(v,c)		((void)__raw_writel(v,c))
#define writeb_relaxed(v,c)		((void)__raw_writeb(v,c))
#define readl(c)				({ unsigned int __v = readl_relaxed(c); __v; })
#define readw(c)				({ unsigned short __v = readw_relaxed(c); __v; })
#define readb(c)				({ unsigned char __v = readb_relaxed(c); __v; })
#define writel(v,c)				({writel_relaxed(v,c); })
#define writeb(v,c)				({writeb_relaxed(v,c); })

#define	WRITE_MEMORY_BARRIER()	{	u32 temp = 0; \
									__asm { MCR p15, 0, temp, c7, c10, 4 }; \
								}
#define	DATA_MEMORY_BARRIER()	{	u32 temp = 0; \
									__asm { MCR p15, 0, temp, c7, c10, 5 }; \
								}
#define	READ_MEMORY_BARRIER DATA_MEMORY_BARRIER


typedef volatile unsigned int		__REG32;
typedef volatile unsigned short int	__REG16;

//#define readb(a)			(*(volatile unsigned char *)(a))
//#define writeb(v,a)			(*(volatile unsigned char *)(a) = (v))

#define __REGw(x)			(*(volatile unsigned long *)(x))
//#define REG_READ(x) 		readl(x)
//#define REG_WRITE(x,data)	writel(data, x);
#define REG_READ(x) 		(*(volatile unsigned int *)(x))
#define REG_WRITE(x,data)	(*(volatile unsigned int *)(x) = (data))

// Register Read/Write MACRO
#define READ_REG8(addr)                     (*(volatile unsigned char *)(addr))
#define READ_REG16(addr)                    (*(volatile unsigned short *)(addr))
#define READ_REG32(addr)                    (*(volatile unsigned int *)(addr))
#define WRITE_REG8(addr, value)             (*(volatile unsigned char *)(addr) = (value))
#define WRITE_REG16(addr, value)            (*(volatile unsigned short *)(addr) = (value))
#define WRITE_REG32(addr, value)            (*(volatile unsigned int *)(addr) = (value))


// Register Bits Control (32Bits)
// set, clear, invert, check, write bits
// addr : register address
// bits : bit size (ex. 0x1, 0x3, 0x7, 0xF)
// val  : write value
// pos  : bit position (0 ~ 31)
#define INV_REG32_BIT(id, addr, pos)				do { \
														((*(__REG32 *)(addr)) ^= (0x1 << (pos))); \
													}while(0)
__attribute__ ((always_inline)) static __inline unsigned int CHK_REG32_BIT_SET(unsigned int addr, 
																				unsigned int pos)
{
	unsigned int value = (*(__REG32 *)(addr));
	return (((value>>pos) & 0x1) ? 1:0);
}
#define CHK_REG32_BIT_CLR(addr, pos)			(!CHK_REG32_BIT_SET(addr, pos))
#define SET_REG32_BITS(addr, bits, pos)			do { \
														((*(__REG32 *)(addr)) |= (((bits))<<(pos))); \
													}while(0)
#define CLR_REG32_BITS(addr, bits, pos)			do { \
														((*(__REG32 *)(addr)) &=~(((bits))<<(pos))); \
													}while(0)
#define INV_REG32_BITS(addr, bits, pos)			do { \
														((*(__REG32 *)(addr)) ^= (((bits))<<(pos))); \
													}while(0)
#define WRITE_REG32_BITS(addr, bits, val, pos)	do { \
														CLR_REG32_BITS(0, (addr), (bits), (pos)); \
														(*(__REG32 *)(addr)) |= (((val))<<(pos)); \
													}while(0)
__attribute__ ((always_inline)) static __inline unsigned int GET_REG32_BITS(unsigned int addr,
																			unsigned int bits,unsigned int pos)
{
	unsigned int value = (*(__REG32 *)(addr));
	return ((value>>pos)&bits);
}

// Register Bits Control (16Bits)
// set, clear, invert, check, write bits
// addr : register address
// bits : bit size (ex. 0x1, 0x3, 0x7, 0xF)
// val  : write value
// pos  : bit position (0 ~ 15)
#define INV_REG16_BIT(id, addr, pos)				do { \
														((*(__REG16 *)(addr)) ^= (0x1 << (pos))); \
													}while(0)
__attribute__ ((always_inline)) static __inline unsigned int CHK_REG16_BIT_SET(unsigned int addr,
																				unsigned int pos)
{
	unsigned int value = (*(__REG16 *)(addr));
	return (((value>>pos) & 0x1) ? 1:0);
}
#define CHK_REG16_BIT_CLR(id, addr, pos)			(!CHK_REG16_BIT_SET(addr, pos))
#define SET_REG16_BITS(addr, bits, pos)			do { \
														((*(__REG16 *)(addr)) |= (((unsigned short)(bits))<<(pos))); \
													}while(0)
#define CLR_REG16_BITS(addr, bits, pos)			do { \
														((*(__REG16 *)(addr)) &=~(((unsigned short)(bits))<<(pos))); \
													}while(0)
#define INV_REG16_BITS(addr, bits, pos)			do { \
														((*(__REG16 *)(addr)) ^= (((unsigned short)(bits))<<(pos))); \
													}while(0)
#define WRITE_REG16_BITS(addr, bits, val, pos)	do { \
														CLR_REG16_BITS(0, (addr), (bits), (pos)); \
														((*(__REG16 *)(addr)) |= (((unsigned short)(val))<<(pos))); \
													}while(0)
__attribute__ ((always_inline)) static __inline unsigned int GET_REG16_BITS(unsigned int addr,
																			unsigned int bits, unsigned int pos)
{
	unsigned int value = (*(__REG16 *)(addr));
	return ((value>>pos)&bits);
}

#endif /* __REG_MACRO_H__ */

