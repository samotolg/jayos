#include "fos.h"
#include "fos_timer.h"
#include "os_sem.h"
#include "os_mutex.h"

extern u32 os_hw_timer_get_global_count();

#define MUMMY_SLEEP
#define DUMMY_SLEEP
#define OS_DELAY

#ifdef OS_SEMAPHORE
static os_sem_type							g_sem;
#endif
#ifdef OS_MUTEX	
static os_mutex_type							g_mutex;
#endif


//------------------------------------------

#ifdef OS_SCHEDULE_ROUND_ROBIN
#define OS_TASK_PRIO_DUMMY					5
#define OS_TASK_PRIO_DUMMY2					5
#else
#define OS_TASK_PRIO_DUMMY					5
#define OS_TASK_PRIO_DUMMY2					6
#endif
//------------------------------------------

#define DUMMY_STACK_SIZE					2048
#define OS_TASK_SLICE_DUMMY					300
#define OS_SIGS_DUMMY_TIMER_WAIT			0x0001

static os_tcb_type 							dummy_tcb;
static u32									dummy_stack[DUMMY_STACK_SIZE];

void dummy_task(u32 param) 
{
	while(1) {
#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		for (int i=0;i<1000;i++) {
			printf("-");
			for (int j=0;j<500; j++);
		}
#ifdef OS_MUTEX
		os_mutex_post(&g_mutex);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_1S*2);
#else
		os_set_timer(TIMER_1S*2, OS_SIGS_DUMMY_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_DUMMY_TIMER_WAIT);
		os_clr_sigs(&dummy_tcb, OS_SIGS_DUMMY_TIMER_WAIT);
#endif		
	};
}

//------------------------------------------

#define DUMMY2_STACK_SIZE					1024
#define OS_TASK_SLICE_DUMMY2				500
#define OS_SIGS_DUMMY2_TIMER_WAIT			0x0040

static os_tcb_type 							dummy2_tcb;
static u32									dummy2_stack[DUMMY2_STACK_SIZE];

void dummy2_task(u32 param) 
{
	while(1) {
#ifdef OS_SEMAPHORE		
		os_sem_wait(&g_sem);
#endif
		for (int i=0;i<2000;i++) {
			printf("|");
			for (int j=0;j<500; j++);
		}
#ifdef OS_SEMAPHORE			
		os_sem_post(&g_sem);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_1S*3);
#else
		os_set_timer(TIMER_1S*3, OS_SIGS_DUMMY2_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_DUMMY2_TIMER_WAIT);
		os_clr_sigs(&dummy2_tcb, OS_SIGS_DUMMY2_TIMER_WAIT);
#endif		
	};
}

//------------------------------------------

#ifdef OS_SCHEDULE_ROUND_ROBIN
#define OS_TASK_PRIO_MUMMY					7
#define OS_TASK_PRIO_MUMMY2					7
#define OS_TASK_PRIO_MUMMY3					7
#define OS_TASK_PRIO_MUMMY4					7
#define OS_TASK_PRIO_MUMMY5					7
#else
#define OS_TASK_PRIO_MUMMY					7
#define OS_TASK_PRIO_MUMMY2					8
#define OS_TASK_PRIO_MUMMY3					9
#define OS_TASK_PRIO_MUMMY4					17
#define OS_TASK_PRIO_MUMMY5					117
#endif

//------------------------------------------

#define MUMMY_STACK_SIZE					1024
#define OS_TASK_SLICE_MUMMY					1000
#define OS_SIGS_MUMMY_TIMER_WAIT			0x0002

static os_tcb_type 							mummy_tcb;
static u32									mummy_stack[MUMMY_STACK_SIZE];

void mummy_task(u32 param) 
{
	while(1) {
#ifdef MUMMY_SLEEP		
		u32 cnt = os_hw_timer_get_global_count();
#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		printf("In Mummy1 Task : %p\n", cnt);
//		for (int i=0;i<500;i++) printf ("1");
#ifdef OS_MUTEX
		os_mutex_post(&g_mutex);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_100MS*2);
#else

		os_set_timer(TIMER_100MS*2, OS_SIGS_MUMMY_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_MUMMY_TIMER_WAIT);
		os_clr_sigs(&mummy_tcb, OS_SIGS_MUMMY_TIMER_WAIT);
#endif
#else

#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		for (int i=0;i<1000;i++)
			printf("1");
#ifdef OS_MUTEX		
		os_mutex_post(&g_mutex);
#endif
#endif		
	};

}

//------------------------------------------

#define MUMMY2_STACK_SIZE					1024
#define OS_TASK_SLICE_MUMMY2				1500
#define OS_SIGS_MUMMY2_TIMER_WAIT			0x0004

static os_tcb_type 							mummy2_tcb;
static u32									mummy2_stack[MUMMY2_STACK_SIZE];

void mummy2_task(u32 param) 
{
	
	while(1) {
#ifdef MUMMY_SLEEP		
		u32 cnt = os_hw_timer_get_global_count();
#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		printf("In Mummy2 Task : %p\n", cnt);
//		for (int i=0;i<500;i++) printf ("2");
#ifdef OS_MUTEX
		os_mutex_post(&g_mutex);
#endif

#ifdef OS_SEMAPHORE		
		os_sem_wait(&g_sem);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_10S);
#else

		os_set_timer(TIMER_10S, OS_SIGS_MUMMY2_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_MUMMY2_TIMER_WAIT);
		os_clr_sigs(&mummy2_tcb, OS_SIGS_MUMMY2_TIMER_WAIT);
#endif
#ifdef OS_SEMAPHORE		
		os_sem_post(&g_sem);
#endif
#else		
		printf("2");
		for (int i=0;i<1000;i++);
#endif		
	};

}

//------------------------------------------

#define MUMMY3_STACK_SIZE					1024
#define OS_TASK_SLICE_MUMMY3				2000
#define OS_SIGS_MUMMY3_TIMER_WAIT			0x0008

static os_tcb_type 							mummy3_tcb;
static u32									mummy3_stack[MUMMY3_STACK_SIZE];

void mummy3_task(u32 param) 
{
	
	while(1) {
#ifdef MUMMY_SLEEP		
		u32 cnt = os_hw_timer_get_global_count();
#ifdef OS_SEMAPHORE		
		os_sem_wait(&g_sem);
#endif

#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		printf("In Mummy3 Task : %p\n", cnt);
//		for (int i=0;i<500;i++) printf ("3");
#ifdef OS_MUTEX
		os_mutex_post(&g_mutex);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_100MS*6);
#else
		os_set_timer(TIMER_100MS*6, OS_SIGS_MUMMY3_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_MUMMY3_TIMER_WAIT);
		os_clr_sigs(&mummy3_tcb, OS_SIGS_MUMMY3_TIMER_WAIT);
#endif		
#ifdef OS_SEMAPHORE		
		os_sem_post(&g_sem);
#endif
#else		
		printf("3");
		for (int i=0;i<1000;i++);
#endif		
	};

}

//------------------------------------------

#define MUMMY4_STACK_SIZE					1024
#define OS_TASK_SLICE_MUMMY4				2500
#define OS_SIGS_MUMMY4_TIMER_WAIT			0x0010

static os_tcb_type 							mummy4_tcb;
static u32									mummy4_stack[MUMMY4_STACK_SIZE];

void mummy4_task(u32 param) 
{
	
	while(1) {
#ifdef MUMMY_SLEEP		
		u32 cnt = os_hw_timer_get_global_count();
#ifdef OS_SEMAPHORE		
				os_sem_wait(&g_sem);
#endif

#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		printf("In Mummy4 Task : %p\n", cnt);
//		for (int i=0;i<500;i++) printf ("4");
#ifdef OS_MUTEX
		os_mutex_post(&g_mutex);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_100MS*8);
#else

		os_set_timer(TIMER_100MS*8, OS_SIGS_MUMMY4_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_MUMMY4_TIMER_WAIT);
		os_clr_sigs(&mummy4_tcb, OS_SIGS_MUMMY4_TIMER_WAIT);
#endif		
#ifdef OS_SEMAPHORE		
		os_sem_post(&g_sem);
#endif		
#else		
		printf("4");
		for (int i=0;i<1000;i++);
#endif		
	};

}

//------------------------------------------

#define MUMMY5_STACK_SIZE					1024
#define OS_TASK_SLICE_MUMMY5				3000
#define OS_SIGS_MUMMY5_TIMER_WAIT			0x0020

static os_tcb_type 							mummy5_tcb;
static u32									mummy5_stack[MUMMY5_STACK_SIZE];

void mummy5_task(u32 param) 
{
	
	while(1) {
#ifdef MUMMY_SLEEP		
		u32 cnt = os_hw_timer_get_global_count();
#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		printf("In Mummy5 Task : %p\n", cnt);

//		for (int i=0;i<500;i++) printf ("5");

#ifdef OS_MUTEX
		os_mutex_post(&g_mutex);
#endif
#ifdef OS_DELAY
		os_delay(TIMER_100MS);
#else

		os_set_timer(TIMER_10MS*10, OS_SIGS_MUMMY5_TIMER_WAIT);
		os_wait_sigs(OS_SIGS_MUMMY5_TIMER_WAIT);
		os_clr_sigs(&mummy5_tcb, OS_SIGS_MUMMY5_TIMER_WAIT);
#endif
#else	
#ifdef OS_MUTEX
		os_mutex_wait(&g_mutex);
#endif
		for (int i=0;i<1000;i++)
			printf("5");
#ifdef OS_MUTEX		
		os_mutex_post(&g_mutex);
#endif
#endif		
	};

}

//------------------------------------------


void task_init()
{
#ifdef OS_SEMAPHORE
	os_sem_init(&g_sem, 2);
#endif
#ifdef OS_MUTEX	
	os_mutex_init(&g_mutex);
#endif
	
	os_create_task(&dummy_tcb, dummy_stack, DUMMY_STACK_SIZE, OS_TASK_PRIO_DUMMY, OS_TASK_SLICE_DUMMY, dummy_task, 0, "dummy");
	os_create_task(&dummy2_tcb, dummy2_stack, DUMMY2_STACK_SIZE, OS_TASK_PRIO_DUMMY2, OS_TASK_SLICE_DUMMY2, dummy2_task, 0, "dummy2");

	os_create_task(&mummy_tcb, mummy_stack, MUMMY_STACK_SIZE, OS_TASK_PRIO_MUMMY, OS_TASK_SLICE_MUMMY, mummy_task, 0, "mummy");
	os_create_task(&mummy2_tcb, mummy2_stack, MUMMY2_STACK_SIZE, OS_TASK_PRIO_MUMMY2, OS_TASK_SLICE_MUMMY2, mummy2_task, 0, "mummy2");
	os_create_task(&mummy3_tcb, mummy3_stack, MUMMY3_STACK_SIZE, OS_TASK_PRIO_MUMMY3, OS_TASK_SLICE_MUMMY3, mummy3_task, 0, "mummy3");
	os_create_task(&mummy4_tcb, mummy4_stack, MUMMY4_STACK_SIZE, OS_TASK_PRIO_MUMMY4, OS_TASK_SLICE_MUMMY4, mummy4_task, 0, "mummy4");
	os_create_task(&mummy5_tcb, mummy5_stack, MUMMY5_STACK_SIZE, OS_TASK_PRIO_MUMMY5, OS_TASK_SLICE_MUMMY5, mummy5_task, 0, "mummy5");
}
