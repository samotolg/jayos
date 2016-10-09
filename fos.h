#ifndef JAYOS_H
#define JAYOS_H

#include "os_types.h"

typedef byte				os_prio_type;
typedef unsigned int		os_sig_type;

typedef enum {OS_STOP, OS_RUN} os_status_type;

typedef struct context_frame_struct {
	u32				spsr;
	u32				r[14];		// r0~r12, r13
	u32				lr;
} os_context_frame_type;

typedef void (*os_task_func_type)( u32 );

typedef void		os_stack_type;

typedef struct os_tcb_link_struct {
	struct os_tcb_struct 	*prev;
	struct os_tcb_struct 	*next;
} os_tcb_link_type;

#define	TASK_NAME_MAX_LEN			10

typedef struct os_tcb_struct {
	u32					*sp;
	u32					*stack_limit;
	u32					slices;
	os_prio_type		prio;
	os_sig_type			sigs;
	os_sig_type			wait;

	char				name[TASK_NAME_MAX_LEN+1];
	u32					hold_slices;

	bool				suspend;
	os_tcb_link_type	link;
} os_tcb_type;


/******************************************************************************
                      Function Prototype Declaration
******************************************************************************/

void os_init();
void os_start();
bool os_create_task(
	os_tcb_type				*p_tcb, 
	os_stack_type			*p_stack, 
	u32		 				stack_size, 
	u32		 				prio,
	u32						slices,
	os_task_func_type 		p_task,
	u32						param,
	char					*name);

os_sig_type os_set_sigs(os_tcb_type *p_tcb, os_sig_type sigs);
os_sig_type os_wait_sigs(os_sig_type sigs);
os_sig_type	os_clr_sigs(os_tcb_type *p_tcb, os_sig_type sigs);

#endif //JAYOS_H
