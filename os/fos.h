#ifndef JAYOS_H
#define JAYOS_H

#include "bsp.h"
#include "fos_config.h"
#include "rtos_type.h"

#define OS_TCB_SIZE
#define OS_TCB_OFFSET_SP					0
#define OS_TCB_OFFSET_SP_LIMIT				(OS_TCB_OFFSET_SP + 4)
#define OS_TCB_OFFSET_SLICES				(OS_TCB_OFFSET_SP_LIMIT + 4)
#define OS_TCB_OFFSET_PRIO					(OS_TCB_OFFSET_SLICES + 4)
#define OS_TCB_OFFSET_SIGS					(OS_TCB_OFFSET_PRIO + 4)
#define OS_TCB_OFFSET_WAIT					(OS_TCB_OFFSET_WAIT+4)

#define OS_CONTEXT_OFFSET_SPSR				0
#define OS_CONTEXT_OFFSET_PC				0x3c

#define OS_INTLOCK()						{ u32  sav = os_int_lock();
#define OS_INTFREE()						if (!sav) os_int_free(); }

#define OS_CRITICAL_SECTION_ENTER()			do {OS_INTLOCK(); os_schedule_lock++; OS_INTFREE(); } while (0);
#define OS_CRITICAL_SECTION_EXIT()			{ OS_INTLOCK(); os_schedule_lock--; OS_INTFREE() } while(0);

/******************************************************************************
                   		   Type Definition
******************************************************************************/
typedef byte								os_prio_type;
typedef unsigned int						os_sig_type;

typedef enum {OS_STOP, OS_RUN} 				os_status_type;

/*-------------------------------------------------------------------
Context Frame Struct
		   Low  -------------
			    |  ELR_EL1  |
		        -------------
			    |  SPSR_EL1 |
		        -------------
			    |    X0     |
			    -------------
		             ...   
		             ...
			    -------------
			    |    X29    |
			    -------------
			    |    LR     |
		        -------------
		        |    XZR    |
		   High -------------
--------------------------------------------------------------------*/
typedef struct context_frame_struct {
	u64							elr;
	u64							spsr;
	u64							x[30];		// x0 ~ x29 
	u64							lr;			// x30 (= link register)
	u64							xzr;		// padding for aligning quadword
} 											os_context_frame_type;

typedef void 								(*os_task_func_type)( u32 );

typedef void								os_stack_type;

typedef struct 								os_tcb_link_struct {
	struct os_tcb_struct 		*prev;
	struct os_tcb_struct 		*next;
} 											os_tcb_link_type;

#define	TASK_NAME_MAX_LEN					10

typedef struct os_tcb_struct {
	u64							*sp;
	u64							*stack_limit;
	u32							slices;
	os_prio_type				prio;
	os_sig_type					sigs;
	os_sig_type					wait;

	char						name[TASK_NAME_MAX_LEN+1];
	u32							hold_slices;

	bool						suspend;
	os_tcb_link_type			link;
#ifdef OS_SCHEDULE_ROUND_ROBIN
	os_tcb_link_type			rr_link;
#endif
#ifdef OS_SEMAPHORE
	struct os_sem_struct		*wait_sem;
	os_tcb_link_type			sem_link;
#endif
#ifdef OS_MUTEX
	struct os_mutex_struct		*wait_mutex;
	os_tcb_link_type			mutex_link;
#ifdef OS_PRIORITY_INHERITANCE
	os_prio_type				prio_org;
#endif
#endif
} 						os_tcb_type;


typedef struct os_timer_struct {
	struct os_timer_struct 		*prev;
	struct os_timer_struct 		*next;
	os_tcb_type					*p_tcb;
	u32							timer;
	u32							sigs;	
} 											os_timer_type;

#ifdef OS_SEMAPHORE
typedef struct os_sem_struct {
	u32						count;  		/* semaphore count */
	os_tcb_type				*list;			/* a tcb list ascending sorted by priority */
} 											os_sem_type;
#endif

#ifdef OS_MUTEX
typedef struct os_mutex_struct {
	u32						id;
	os_tcb_type				*list;			/* a tcb list ascending sorted by priority */
} 											os_mutex_type;
#endif


/******************************************************************************
                      External Functions Used in OS
******************************************************************************/
extern void 	os_schedule();
extern u32		os_int_lock();
extern u32		os_int_free();

extern void 	os_hw_timer_start();
extern void 	os_hw_timer_init();
extern u32 		os_hw_timer_get_count(u8 ch);
extern u32 		os_hw_timer_get_global_count();
extern void 	os_hw_timer_enable(u8 ch, u32 count, void (*timer_cb)(int));
extern void 	os_hw_timer_disable(u8 ch);
extern void 	os_hw_timer_clear_sig(u8 ch);
extern void 	os_hw_timer_pause(u8 ch);
extern void 	os_hw_timer_resume(u8 ch);


/******************************************************************************
		                      OS API to provide
******************************************************************************/
void 			os_init();
void 			os_start();
void 			os_create_task(
						os_tcb_type				*p_tcb, 
						os_stack_type			*p_stack, 
						u32		 				stack_size, 
						u32		 				prio,
						u32						slices,
						os_task_func_type 		p_task,
						u64						param,
						char					*name);
os_sig_type		os_set_sigs(os_tcb_type *p_tcb, os_sig_type sigs);
os_sig_type 	os_wait_sigs(os_sig_type sigs);
os_sig_type		os_clr_sigs(os_tcb_type *p_tcb, os_sig_type sigs);
os_timer_type* 	os_set_timer(u32 timer, os_sig_type sigs);
void			os_delay(u32 timer);
void		 	os_set_status(os_tcb_type *p_task);


extern u64 		os_schedule_lock;

#endif //JAYOS_H
