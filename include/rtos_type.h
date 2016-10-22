#ifndef __RTOS_TYPE_H__
#define __RTOS_TYPE_H__

#define	TRUE					1
#define	FALSE					0
#define NULL					0

typedef unsigned char           bool;         /* Unsinged 16 bit value type. */
typedef signed char             t_int8;
typedef unsigned char           t_uint8;
typedef short int                       t_int16;
typedef unsigned short int      t_uint16;
typedef int                                     t_int32;
typedef unsigned int            t_uint32;
typedef t_uint16                        t_wchar;
typedef t_int32                         t_boolean;
typedef int                                     t_int;
typedef unsigned int            t_uint;
typedef long long                       t_longlong;

typedef signed char             s8;
typedef unsigned char           u8;
typedef short int                       s16;
typedef unsigned short int      u16;
typedef int                                     s32;
typedef unsigned int            u32;
typedef unsigned int            __u32;
typedef long long                       s64;
typedef unsigned long long      u64;

typedef signed char             int8;
typedef unsigned char           uint8;
typedef short int                       int16;
typedef unsigned short int      uint16;
typedef int                                     int32;
typedef unsigned int            uint32;
typedef long long                       int64;
typedef unsigned long long      uint64;
typedef unsigned long long      sector_t;

typedef s8                                      CHAR;           /* 8bit integer */
typedef u8                                      UCHAR;
typedef u8                                      BYTE;

typedef s16                                     SHORT;          /* 16bit integer */
typedef u16                                     USHORT;
typedef u16                                     WORD;
typedef u16                                     WCHAR;

typedef s32                                     LONG;           /* 32bit integer */
typedef u32                                     ULONG;
typedef u32                                     DWORD;
typedef u32                                     UINT;


typedef unsigned long           dword;                          /* Unsigned 32 bit value type. */
typedef unsigned char           byte;         /* Unsigned 8  bit value type. */
typedef unsigned short          word;         /* Unsinged 16 bit value type. */

#endif /* __RTOS_TYPE_H__ */

