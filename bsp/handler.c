#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "hal_interrupt.h"

/* Current EL with SPx */
#define AARCH64_EXC_SYNC_SPX      (0x11)
#define AARCH64_EXC_IRQ_SPX       (0x12)
#define AARCH64_EXC_FIQ_SPX       (0x13)
#define AARCH64_EXC_SERR_SPX      (0x14)

typedef struct _exception_frame
{
    uint64_t exc_type;
    uint64_t exc_esr;
    uint64_t exc_elr;
    uint64_t exc_spsr;
    uint64_t x29;
    uint64_t x30;
    uint64_t x18;
    uint64_t x19;
    uint64_t x17;
    uint64_t x16;
    uint64_t x14;
    uint64_t x15;
    uint64_t x12;
    uint64_t x13;
    uint64_t x10;
    uint64_t x11;
    uint64_t x8;
    uint64_t x9;
    uint64_t x6;
    uint64_t x7;
    uint64_t x4;
    uint64_t x5;
    uint64_t x2;
    uint64_t x3;
    uint64_t x0;
    uint64_t x1;
    
} exception_frame;

void handle_exception(exception_frame *exc)
{
    debug_printf("An exception occur:\n");
    debug_printf("exc_type: ");
    debug_printf("%x", exc->exc_type);
    debug_printf("\nESR: "); debug_printf("%x", exc->exc_esr);
    debug_printf(" ELR: "); debug_printf("%x", exc->exc_elr);
    debug_printf(" SPSR: "); debug_printf("%x", exc->exc_spsr);
    debug_printf("\n x0: "); debug_printf("%x", exc->x0);
    debug_printf("  x1: "); debug_printf("%x", exc->x1);
    debug_printf("  x2: "); debug_printf("%x", exc->x2);
    debug_printf("  x3: "); debug_printf("%x", exc->x3);
    debug_printf("\n x4: "); debug_printf("%x", exc->x4);
    debug_printf("  x5: "); debug_printf("%x", exc->x5);
    debug_printf("  x6: "); debug_printf("%x", exc->x6);
    debug_printf("  x7: "); debug_printf("%x", exc->x7);
    debug_printf("\n x8: "); debug_printf("%x", exc->x8);
    debug_printf("  x9: "); debug_printf("%x", exc->x9);
    debug_printf(" x10: "); debug_printf("%x", exc->x10);
    debug_printf(" x11: "); debug_printf("%x", exc->x11);
    debug_printf("\nx12: "); debug_printf("%x", exc->x12);
    debug_printf(" x13: "); debug_printf("%x", exc->x13);
    debug_printf(" x14: "); debug_printf("%x", exc->x14);
    debug_printf(" x15: "); debug_printf("%x", exc->x15);
    debug_printf("\nx16: "); debug_printf("%x", exc->x16);
    debug_printf(" x17: "); debug_printf("%x", exc->x17);
    debug_printf(" x18: "); debug_printf("%x", exc->x18);
    debug_printf(" x19: "); debug_printf("%x", exc->x19);
    debug_printf("\nx29: "); debug_printf("%x", exc->x29);
    debug_printf(" x30: "); debug_printf("%x", exc->x30);
}

// TODO: for test
bool once = false;
void common_trap_handler(exception_frame *exc)
{
    hal_interrupt_run_handler();

    if ( ( exc->exc_type & 0xff ) == AARCH64_EXC_SYNC_SPX ) {
        if (once == false)
        {
            debug_printf("AARCH64_EXC_SYNC_SPX\n");
            handle_exception(exc);
            once = true;
        }
    }
}
