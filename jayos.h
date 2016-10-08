#ifndef JAYOS_H
#define JAYOS_H

#include "jayos_types.h"

typedef byte				os_prio_type;
typedef unsigned int		os_sig_type;

typedef enum {OS_STOP, OS_RUN} os_status_type;

typedef struct context_frame_struct {
	u32				lr;
	u32				r[14];		// r0~r12, r13
	u32				spsr;
} os_context_frame_type;



typedef struct os_tcb_link_struct {
	struct os_tcb_struct 	*prev;
	struct os_tcb_struct 	*next;
} os_tcb_link_type;

typedef struct os_tcb_struct {
	byte				*sp;
	byte				*stack_limit;
	u32					slices;
	os_prio_type		prio;
	os_sig_type			sigs;
	os_sig_type			wait;

	bool				suspend;
	os_tcb_link_type	link;
} os_tcb_type;

#endif //JAYOS_H
