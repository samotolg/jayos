#ifndef __RTOS_TYPE_H__
#define __RTOS_TYPE_H__

#define	TRUE					1
#define	FALSE					0
#define NULL					0

typedef unsigned char           bool;         /* Unsinged 16 bit value type. */
typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef short int               int16_t;
typedef unsigned short int      uint16_t;
typedef int                     int32_t;
typedef unsigned int            uint32_t;
typedef long int                int64_t;
typedef unsigned long int       uint64_t;
typedef uint16_t                wchar_t;
typedef int32_t                 boolean_t;
typedef int                     int_t;
typedef unsigned int            uint_t;
typedef long long               longlong_t;

typedef signed char             s8;
typedef unsigned char           u8;
typedef short int               s16;
typedef unsigned short int      u16;
typedef int                     s32;
typedef unsigned int            u32;
typedef unsigned int            __u32;
typedef long long               s64;
typedef unsigned long long      u64;

typedef signed char             int8;
typedef unsigned char           uint8;
typedef short int               int16;
typedef unsigned short int      uint16;
typedef int                     int32;
typedef unsigned int            uint32;
typedef long long               int64;
typedef unsigned long long      uint64;
typedef unsigned long long      sector_t;

typedef s8                      CHAR;           /* 8bit integer */
typedef u8                      UCHAR;
typedef u8                      BYTE;

typedef s16                     SHORT;          /* 16bit integer */
typedef u16                     USHORT;
typedef u32                     WORD;
typedef u32                     WCHAR;

typedef s64                     LONG;           /* 32bit integer */
typedef u64                     ULONG;
typedef u64                     DWORD;
typedef u64                     UINT;


typedef unsigned long           dword;                          /* Unsigned 32 bit value type. */
typedef unsigned char           byte;         /* Unsigned 8  bit value type. */
typedef unsigned short          word;         /* Unsinged 16 bit value type. */

#endif /* __RTOS_TYPE_H__ */

