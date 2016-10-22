/**
 * \file
 *
 * $Copyright (c) 2008-2009, ARM Ltd. All rights reserved.

 * -------------------------------------------------------

 * File:     armv7_gic.h

 * $
 * 
 * \brief Interrupt Controller driver header
 * \defgroup GIC_DRIVER Primary Generic Interrupt Controller Driver
 *  
 */


#ifndef __GIC_H__
#define __GIC_H__

#include "rtos_type.h"
#include "artik_board_config.h"

/**
 * \brief ID of 'spurious' interrupt
 *
 * This ID is returned when acknowledging an interrupt, if no interrupt is pending.
 * This is also returned when acknowledging an interrupt that has become masked due
 * to priority change but was already pending (sent to the CPU).
 */
#define SPURIOUS_INTERRUPT 1023


/*
 * Number of interrupts implemented in the GIC
 *
 * The number of interrupts that the Distributor can handle is IMPLEMENTATION DEFINED.
 * It can be up to 1020, with up to 4 CPUs being supported (expansion space reserved for
 * up to 8 CPUs)
 */
#define NO_OF_INTERRUPTS_IMPLEMENTED 96


/*
 * Description:
 * Number of 32 bit registers necessary in order to allocate enough space
 * for 1020 irqs (1 bit each) for the following:
 * - Enable Set / Enable Clear
 * - Pending Set / Pending Clear
 * - Active Bit
 */
#define GIC_NUM_REGISTERS 32

 
/*
 * Description:
 * Number of 4 byte registers necessary in order to allocate enough space
 * for 1020 irqs (8 bit each) for the priority field
 */
#define GIC_PRIORITY_REGISTERS 256 

 
/*
 * Description:
 * Number of 4 byte registers necessary in order to allocate enough space
 * for 1020 irqs (8 bit each) for the CPU targets field
 */
#define GIC_TARGET_REGISTERS 256 


/*
 * Description:
 * Number of 4 byte registers necessary in order to allocate enough space
 * for 1020 irqs (8 bit each) for the configuration field
 */
#define GIC_CONFIG_REGISTERS 64 


/*
 * Description:
 * Number of Peripheral Identification Registers
 */
#define GIC_PERIPHERAL_ID 4


/*
 * Description:
 * Number of PrimeCell Hardcoded Peripherals Register
 * Four words, each of which contains a hardcoded byte reset value
 */
#define GIC_PRIMECELL_ID 4


/*
 * Description:
 * Reserved address space of the GIC controller (size expressed in 32 bit words)
 */
#define GIC_RESERVED_BEFORE_ENABLE_SET       62
#define GIC_RESERVED_BEFORE_PRIORITY         32
#define GIC_RESERVED_BEFORE_IDENTIFICATION   55
#define GIC_IMPLEMENTATION_DEFINED_DISTR     64
#define GIC_RESERVED_BEFORE_SOFT_INT        128


/*
 * Description:
 * Mask for Inter Processor Interrupt ID
 */
#define IPI_MASK           0x0000000F


/*
 * Description:
 * Mask for interrupt ID (used with acknowledge, highest pending, software and end of interrupt registers
 */
#define INTERRUPT_MASK     0x000003FF  


/*
 * Description:
 * Mask for CPU source ID (used with highest pending and end of interrupt registers
 */
#define CPUID_MASK         0x00000C00


/*
 * Description:
 * Mask for Binary Point register
 */
#define BINARY_POINT_MASK  0x00000007


/*
 * Description:
 * Mask for Priority Mask and Running Interrupt registers
 */
#define PRIORITY_MASK_MASK 0x000000F0


/*
 * Description:
 * Shift required to locate the CPU_ID field in the interface registers
 */
#define CPUID_SHIFT        10


/*
 * Description:
 * Shift required to locate the priority field in the interface registers
 */
#define PRIORITY_SHIFT      4


/*
 * Description:
 * Shift required to locate the CPU target list field in the Software Interrupt register
 */
#define IPI_TARGET_SHIFT   16


/*
 * Description:
 * Shift required to locate the target list filter field in the Software Interrupt register
 */
#define IPI_TARGET_FILTER_SHIFT 24


/*
 * Description:
 * OS timer (context switch) interrupt ID
 * Using IPI 0 with priority 0 - this makes it never pre-emptable
 */
#define SCHEDULER_IPI IPI_0


/*
 * Description:
 * Interrupt sent to a CPU to start/stop the monitoring of the specified event
 * 
 */
#define EVENTS_IPI IPI_15


/*
 * Description:
 * Interrupt sent to a CPU to enable/disable L1 (for that cpu) or L2 caches
 * 
 */
#define CACHES_IPI IPI_14


/**
 * Target list filter for Software Interrupt register
 */
#define USE_TARGET_LIST   0x0 /**< Interrupt sent to the CPUs listed in CPU Target List */
#define ALL_BUT_SELF      0x1 /**< CPU Target list ignored, interrupt is sent to all but requesting CPU */
#define SELF              0x2 /**< CPU Target list ignored, interrupt is sent to requesting CPU only */


/**
 * Binary point bit values
 */
#define COMPARE_ALL       0x3 /**< All priority bits are compared for pre-emption */
#define COMPARE_7_6_5     0x4 /**< Only bits [7:5] of the priority are compared for pre-emption */
#define COMPARE_7_6       0x5 /**< Only bits [7:6] of the priority are compared for pre-emption */
#define COMPARE_7         0x6 /**< Only bit [7] of the priority is compared for pre-emption */
#define NO_PREEMPTION     0x7 /**< No pre-emprion is performed */


/**
 * IPIs IDs
 */
#define IPI_0      0 /**< Inter Processor Interrupt ID 0 */
#define IPI_1      1 /**< Inter Processor Interrupt ID 1 */
#define IPI_2      2 /**< Inter Processor Interrupt ID 2 */
#define IPI_3      3 /**< Inter Processor Interrupt ID 3 */
#define IPI_4      4 /**< Inter Processor Interrupt ID 4 */
#define IPI_5      5 /**< Inter Processor Interrupt ID 5 */
#define IPI_6      6 /**< Inter Processor Interrupt ID 6 */
#define IPI_7      7 /**< Inter Processor Interrupt ID 7 */
#define IPI_8      8 /**< Inter Processor Interrupt ID 8 */
#define IPI_9      9 /**< Inter Processor Interrupt ID 9 */
#define IPI_10    10 /**< Inter Processor Interrupt ID 10 */
#define IPI_11    11 /**< Inter Processor Interrupt ID 11 */
#define IPI_12    12 /**< Inter Processor Interrupt ID 12 */
#define IPI_13    13 /**< Inter Processor Interrupt ID 13 */
#define IPI_14    14 /**< Inter Processor Interrupt ID 14 */
#define IPI_15    15 /**< Inter Processor Interrupt ID 15 */


/**
 * Masks for CPU selection bitfield for IPI primitives
 */
#define IPI_MASK_CPU0     0x1 /**< Mask for CPU0 to use in CPU selection bitfield for IPI primitives */
#define IPI_MASK_CPU1     0x2 /**< Mask for CPU1 to use in CPU selection bitfield for IPI primitives */
#define IPI_MASK_CPU2     0x4 /**< Mask for CPU2 to use in CPU selection bitfield for IPI primitives */
#define IPI_MASK_CPU3     0x8 /**< Mask for CPU3 to use in CPU selection bitfield for IPI primitives */

/**
 * Exception vector table base address
 */
#define VECTOR_BASE 0x0

/**
 * OS timer (context switch) interrupt ID
 * Using IPI 0 with priority 0 - this makes it never pre-emptable
 */
#define SCHEDULER_IPI IPI_0


/**
 * Some priority's default values
 *
 * The CPU interface sends an interrupt to the CPU if its priority (set in
 * the distributor) is strictly higher than the mask set in the Priority Mask Register.
 * One conseguence of the strict comparison is that a Pending interrupt with the lowest 
 * possible priority (0x0F), never causes the assertion of an interrupt request to MP11 CPUs.
 *
 */
#define HIGHEST_PRIORITY       0x0  /**< */
#define LOWEST_PRIORITY        0xE  /**< */
#define DEFAULT_PRIORITY       0xF  /**< */ /* At reset */
#define RESCHEDULE_PRIORITY    LOWEST_PRIORITY /**< */


/**
 * Enable/Disable macros (used as a parameter in interrupt_enable() )
 */
#define ENABLE        1 /**< Interrupt enable. Used with interrupt_enable() primitive. */
#define DISABLE       0 /**< Interrupt disable. Used with interrupt_enable() primitive. */


/**
 * Set/Unset macros (used as a parameter in interrupt_target_set() )
 */
#define SET           1 /**< Set macro (used as a parameter in interrupt_target_set() */
#define UNSET         0 /**< Un-Set macro (used as a parameter in interrupt_target_set() */


/** 
 * API's functions return error codes
 * Certain error codes are detailed inside the function which returns them.
 *
 */
#define GIC_OK       0    /**< No error */
#define GIC_INVAL    1    /**< Invalid argument */
#define GIC_ERROR    2    /**< Generic error */


/*
 * Description:
 * Access structure for set/clear set of registers
 */
struct set_and_clear_regs
{
	volatile u32 set[GIC_NUM_REGISTERS];
//XXX#if defined(CONFIG_CORE_D5_CA9MP)
//  const u32 reserved_set[7];
//#endif
	volatile u32 clear[GIC_NUM_REGISTERS];
//#if defined(CONFIG_CORE_D5_CA9MP)
//  const u32 reserved_clear[7];
//#endif

};


/*
 * Interrupt Distributor access structure
 *
 */
typedef struct
{
    /* 0x000 */  u32 control; /* Interrupt Distributor Control Register (R/W) */
    /* 0x004 */  volatile u32 const controller_type; /* Interrupt Controller Type Register (RO) */
#if defined(CONFIG_CORE_D5_CA9MP)
    /* 0x008 */  volatile u32 identification;								 /* destributor_implementer_identification register */
    /* 0x00C */  const u32 reserved1[29];											 /* Reserved */
    /* 0x080 */  volatile u32 int_security[GIC_NUM_REGISTERS]; /* interrupt security registers */
#else
    /* 0x008 */  const u32 reserved1[GIC_RESERVED_BEFORE_ENABLE_SET]; /* Reserved */
#endif
    /* 0x100 */  struct set_and_clear_regs enable; /* Interrupt Set/Clear Enable Registers (R/W) */
    /* 0x200 */  struct set_and_clear_regs pending; /* Interrupt Set/Clear Pending Registers (R/W) */
    /* 0x300 */  volatile u32 const active[GIC_NUM_REGISTERS]; /* Interrupt Active Bit Registers (R/W) */
    /* 0x380 */  const u32 reserved2[GIC_RESERVED_BEFORE_PRIORITY]; /* Reserved */
    /* 0x400 */  volatile u32 priority[GIC_PRIORITY_REGISTERS]; /* Interrupt Priority Registers (R/W) */
    /* 0x800 */  volatile u32 target[GIC_TARGET_REGISTERS]; /* Interrupt CPU Target Registers (R/W) */
    /* 0xC00 */  volatile u32 configuration[GIC_CONFIG_REGISTERS]; /* Interrupt Configuration Registers (R/W) */
#if defined(CONFIG_CORE_D5_CA9MP)
    /* 0xD00 */  volatile u32 ppi_status ; 											/* ppi status reg. */
    /* 0xD04 */  volatile u32 spi_status[GIC_NUM_REGISTERS-1] ; /* spi status reg */
    /* 0xD80 */  const u32 reserved3[96]; /* Reserved */
#else
    /* 0xD00 */  const u32 reserved3[GIC_RESERVED_BEFORE_SOFT_INT]; /* Reserved */
#endif
    /* 0xF00 */  volatile u32 software_int; /* Software Interrupt Register (RO) */
    /* 0xF04 */  const u32 reserved4[GIC_RESERVED_BEFORE_IDENTIFICATION]; /* Reserved */
    /* 0xFE0 */  volatile u32 const peri_id[GIC_PERIPHERAL_ID]; /* Peripheral Identification Registers (RO) */
    /* 0xFF0 */  volatile u32 const primecell_id[GIC_PRIMECELL_ID]; /* PrimeCell Identification Registers (RO) */
} interrupt_distributor;



/*
 * CPU Interface access structure
 *
 * These registers are aliased for each CPU
 */
typedef struct
{
    /* 0x00 */  volatile u32 control; /* Control Register (R/W) */
    /* 0x04 */  volatile u32 priority_mask; /* Priority Mask Register (R/W) */
    /* 0x08 */  volatile u32 binary_point; /* Binary Point Register (R/W) */
    /* 0x0C */  volatile u32 const interrupt_ack; /* Interrupt Acknowledge Register (R) */
    /* 0x10 */  volatile u32 end_of_interrupt; /* End of Interrupt Register (W) */
    /* 0x14 */  volatile u32 const running_priority; /* Running Priority Register (R) */
    /* 0x18 */  volatile u32 const highest_pending; /* Highest Pending Interrupt Register (R) */
#if defined(CONFIG_CORE_D5_CA9MP)
    /* 0x1C */  volatile u32 aliased_non_secure_binary_point; /* Highest Pending Interrupt Register (R) */
    /* 0x20 */  const u32 reserved_ci[55]; /* Reserved */
    /* 0xFC */  volatile u32 const implementer_identification; /* Highest Pending Interrupt Register (R) */
#endif
} cpu_interface;


/*
 * Description:
 * Type definition of the interrupt handler
 */
//XXX GIC typedef void *tHandler(s32, s32);
typedef void	(*irqHandler)(s32);


/***** See corresponding source file for description *****/
extern void raw_irq_handler(void);


/**
 * Install low level IRQ handler
 *
 * Only one CPU should call this routine
 * 
 * \remark Interrupts will be disbaled after return from this routine
 *
 */
void intr_install_handler(void);


/**
 * Set the priority of the specified interrupt
 *
 * Priority zero is the highest priority. The lowest priority is priority 0xF.
 *
 * \param[in] interrupt The interrupt ID of which set the priority
 * \param[in] priority  The priority to be set
 *
 * \return The previous priority value for this interrupt
 * 
 */
u32 intr_priority_set(u32 int_id, u32 priority);


/**
 * Get an interrupt's priority
 *
 * \param[in] interrupt The ID of the interrupt
 *
 * \return The priority of the specified interrupt
 * 
 */
u32 intr_priority_get(u32 int_id);


void intr_target_set(u32 int_id, u32 cpu, u32 set);


/**
 * Retrieve the targets of the specified interrupt
 *
 * \param[in] interrupt The ID of the interrupt
 *
 * Return Value:
 * \return The CPU target mask for the specified interrupt
 * 
 */
u32 intr_target_get(u32 int_id);


/**
 * Get the priority level of the last acknowledged (but not completed)
 * interrupt on the calling CPU
 *
 * \return The priority level
 * 
 */
u32 intr_running_priority(void);


/**
 * Get the interrupt ID of the highest priority pending interrupt on the calling CPU.
 *
 * Returns 'spurious' interrupt if no interrupt pending
 *
 * \return The interrupt ID
 * 
 */
u32 intr_highest_pending_ID_get(void);


/**
 * Get the source CPU of the highest priority pending interrupt on the calling CPU.
 *
 * \return The source CPU ID.
 * 
 */
u32 intr_highest_pending_source_get(void);


/**
 * Set the binary point for the CPU Interface of the calling CPU
 *
 * \remark The Binary point is used to ignore a certain number of bits in the
 * priority comparison made in the CPU interface for pre-emption.
 *
 * \param[in] binary_point New binary point value, any of the following:
 * (COMPARE_ALL, COMPARE_7_6_5, COMPARE_7_6, COMPARE_7, NO_PREEMPTION)
 *
 * \return The previous binary point value
 * 
 */
u32 intr_binary_point_set(u32 binary_point);


/**
 * Get the binary point for the CPU Interface of the calling CPU
 *
 * \return The binary point
 * 
 */
u32 intr_binary_point_get(void);


/**
 * Set the priority mask for the CPU Interface of the calling CPU
 *
 * \remark Priority zero is the highest priority. The lowest priority is priority 0xF.
 *
 * \param[in] priority_mask The new priority mask value
 *
 * \return Previous priority mask value 
 * 
 */
u32 intr_priority_mask_set(u32 priority_mask);


/**
 * Retrieve the current priority mask value for the CPU interface of the calling CPU
 *
 * \return The priority mask
 * 
 */
u32 intr_priority_mask_get(void);


/**
 * Retrieve state of the primary GIC
 *
 * \retval TRUE if GIC enabled, FALSE if not
 * 
 */
bool gic_enabled(void);

/*
 * software interrupt generate
 * interrupt : SPI number
 */
void intr_sgi_send(u32 interrupt);

/**
 * Send an IPI to a bitmask of CPUs
 *
 * \param[in] target_mask A bitwise OR of the following values:
 *   - IPI_MASK_CPU0 - select CPU0
 *   - IPI_MASK_CPU1 - select CPU1
 *   - IPI_MASK_CPU2 - select CPU2
 *   - IPI_MASK_CPU3 - select CPU3 
 * \param[in] interrupt IPI ID to be asserted (0x0 to 0xF)
 *
 * \return No return value
 * 
 */
void ipi_send(u32 target_mask, u32 int_id);


/**
 * Send an IPI to the requesting CPU
 *
 * \param[in] interrupt IPI ID to be asserted (0x0 to 0xF)
 *
 * \return No return value
 * 
 */
void ipi_broadcast_self(u32 int_id);


/**
 * Send an IPI to all but the requesting CPU
 *
 * \param[in] interrupt IPI ID to be asserted (0x0 to 0xF)
 *
 * \return No return value * 
 */
void ipi_broadcast_all_but_self(u32 int_id);


/**
 * Enable (enable=1) or disable (enable=0) an interrupt
 *
 * \param[in] interrupt The ID of the interrupt
 * \param[in] enable    Enable or disable (boolean)
 *
 * \return No return value
 * 
 */
void intr_enable(u32 int_id);

/**
 * Enable (enable=1) or disable (enable=0) an interrupt
 *
 * \param[in] interrupt The ID of the interrupt
 * \param[in] enable    Enable or disable (boolean)
 *
 * \return No return value
 * 
 */
void intr_disable(u32 int_id);

void intr_disable_all(void);

void intr_clear(u32 int_id);

/**
 * Install a specific interrupt's handler 
 *
 * \param[in] interrupt The ID of the interrupt
 * \param[in] handler   The handling routine
 *
 * \return No return value
 * 
 */
void intr_setup_isr(u32 int_id, irqHandler handler);


/**
 * Specify an interrupt to be routed to a target CPU (set=1)
 * or stop an interrupt being routed to a target CPU (set=0)
 *
 * \param[in] interrupt The ID of the interrupt
 * \param[in] cpu       Target CPU
 * \param[in] set       Set or unset (boolean)
 *
 * \return No return value
 * 
 */

#endif /* __GIC_H__ */


