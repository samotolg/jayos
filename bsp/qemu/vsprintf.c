
#include <stdarg.h>

#define PRINTF_BUF_LEN 256

#define ZEROPAD    1        /* pad with zero */
#define SIGN       2        /* unsigned/signed long */
#define PLUS       4        /* show plus */
#define SPACE      8        /* space if plus */
#define LEFT       16       /* left justified */
#define SMALL      32       /* Must be 32 == 0x20 */
#define SPECIAL    64       /* 0x */

#ifndef NULL
#define NULL 0
#endif


/* for setting the output string */
static inline int isdigit(int ch);
static int skip_atoi(const char **s);
static char *copychar(char *str, char chr, int *curlen);
static char *number(char *str, long num, int base, int size, int precision, int type, int *pcurlen);

static inline int isdigit(int ch)
{
    return (ch >= '0') && (ch <= '9');
}

static int skip_atoi(const char **s)
{
    int i = 0;

    while (isdigit(**s))
    {
        i = i * 10 + *((*s)++) - '0';
    }

    return i;
}

int prnt_strnlen(const char *s, int maxlen)
{
    const char *es = s;
    while (*es && maxlen) 
    {
        es++;
        maxlen--;
    }

    return (es - s);
}

#define prnt_do_div(n,base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

static char *copychar(char *str, char chr, int *curlen)
{
    if (*curlen < PRINTF_BUF_LEN-1) 
    {
        *str++ = chr;
        (*curlen)++;
    }

    return str;
}

static char *number(char *str, long num, int base, int size, int precision, int type, int *pcurlen)
{
    /* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
    static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

    char tmp[66];
    char c, sign, locase;
    int i;

    /* locase = 0 or 0x20. ORing digits or letters with 'locase'
     * produces same digits or (maybe lowercased) letters */
    locase = (type & SMALL);
    if (type & LEFT)
    {
        type &= ~ZEROPAD;
    }
    if (base < 2 || base > 36)
    {
        return NULL;
    }
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) 
    {
        if (num < 0) 
        {
            sign = '-';
            num = -num;
            size--;
        } 
        else if (type & PLUS) 
        {
            sign = '+';
            size--;
        } 
        else if (type & SPACE) 
        {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL) 
    {
        if (base == 16)
        {
            size -= 2;
        }
        else if (base == 8)
        {
            size--;
        }
    }
    i = 0;
    if (num == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
        {
            tmp[i++] = (digits[prnt_do_div(num, base)] | locase);
        }
    }
    if (i > precision)
    {
        precision = i;
    }
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
    {
        while (size-- > 0)
        {
            str = copychar(str, ' ', pcurlen);
        }
    }
    if (sign)
    {
        str = copychar(str, sign, pcurlen);
    }
    if (type & SPECIAL)
    {
        if (base == 8)
        {
            str = copychar(str, '0', pcurlen);
        }
        else if (base == 16) 
        {
            str = copychar(str, '0', pcurlen);
            str = copychar(str, ('X' | locase), pcurlen);
        }
    }
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            str = copychar(str, c, pcurlen);
        }
    }
    while (i < precision--)
    {
        str = copychar(str, '0', pcurlen);
    }
    while (i-- > 0)
    {
        str = copychar(str, tmp[i], pcurlen);
    }
    while (size-- > 0)
    {
        str = copychar(str, ' ', pcurlen);
    }

    return str;
}

int d_vsprintf(char *buf, const char *fmt, va_list args)
{
    int len, curlen;
    unsigned long num;
    int i, base;
    char *str;
    const char *s;

    int flags;          /* flags to number() */

    int field_width;    /* width of output field */
    int precision;      /* min. # of digits for integers; max
                           number of chars for from string */
    int qualifier;      /* 'h', 'l', or 'L' for integer fields */

    curlen = 0;

    for (str = buf; *fmt; ++fmt) 
    {
        if (*fmt != '%') 
        {
            str = copychar(str, *fmt, &curlen);
            continue;
        }

        /* process flags */
        flags = 0;
          repeat:
        ++fmt;        /* this also skips first '%' */
        switch (*fmt) 
        {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt))
        {
            field_width = skip_atoi(&fmt);
        }
        else if (*fmt == '*') 
        {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0) 
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') 
        {
            ++fmt;
            if (isdigit(*fmt))
            {
                precision = skip_atoi(&fmt);
            }
            else if (*fmt == '*') 
            {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
            {
                precision = 0;
            }
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') 
        {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) 
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    str = copychar(str, ' ', &curlen);
                }
            }
            str = copychar(str, (unsigned char)va_arg(args, int), &curlen);
            while (--field_width > 0)
            {
                str = copychar(str, ' ', &curlen);
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            len = prnt_strnlen(s, precision);

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    str = copychar(str, ' ', &curlen);
                }
            }
            for (i = 0; i < len; ++i)
            {
                str = copychar(str, *s++, &curlen);
            }
            while (len < field_width--)
            {
                str = copychar(str, ' ', &curlen);
            }
            continue;

        case 'p':
            if (field_width == -1) 
            {
                field_width = 2 * sizeof(void *);
                flags |= ZEROPAD;
            }
            str = number(str,
                     (unsigned long)va_arg(args, void *), 16,
                     field_width, precision, flags, &curlen);
            continue;

        case 'n':
            if (qualifier == 'l') 
            {
                long *ip = va_arg(args, long *);
                *ip = (str - buf);
            } 
            else 
            {
                int *ip = va_arg(args, int *);
                *ip = (str - buf);
            }
            continue;

        case '%':
            str = copychar(str, '%', &curlen);
            continue;

            /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'x':
            flags |= SMALL;
        case 'X':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            str = copychar(str, '%', &curlen);
            if (*fmt)
            {
                str = copychar(str, *fmt, &curlen);
            }
            else
            {
                --fmt;
            }
            continue;
        }
        if (qualifier == 'l')
        {
            num = va_arg(args, unsigned long);
        }
        else if (qualifier == 'h') 
        {
            num = (unsigned short)va_arg(args, int);
            if (flags & SIGN)
            {
                num = (short)num;
            }
        } 
        else if (flags & SIGN)
        {
            num = va_arg(args, int);
        }
        else
        {
            num = va_arg(args, unsigned int);
        }
        str = number(str, num, base, field_width, precision, flags, &curlen);
    }
    *str = '\0';

    return str - buf;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	int size;
	va_start(ap, fmt);
	size = d_vsprintf(buf, (const char *)fmt, ap);
	va_end(ap);
	return size;
}

