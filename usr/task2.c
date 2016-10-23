/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
   threads of different priorities, using a message queue, semaphore, mutex, event flags group, 
   byte pool, and block pool.  */

#include   	"fos.h"
#include 	"os_sem.h"
#include	"os_mutex.h"
#include 	"fos_timer.h"

#define     DEMO_STACK_SIZE         512
#define     DEMO_BYTE_POOL_SIZE     9120
#define     DEMO_BLOCK_POOL_SIZE     100
#define     DEMO_QUEUE_SIZE          100


/* Define the ThreadX object control blocks...  */
static os_tcb_type 							thread_0;
//static os_tcb_type 							thread_1;
//static os_tcb_type 							thread_2;
static os_tcb_type 							thread_3;
static os_tcb_type 							thread_4;
static os_tcb_type 							thread_5;
static os_tcb_type 							thread_6;
static os_tcb_type 							thread_7;
os_sem_type									semaphore_0;
os_mutex_type								mutex_0;

static u32									thread_0_stack[DEMO_STACK_SIZE];
//static u32									thread_1_stack[DEMO_STACK_SIZE];
//static u32									thread_2_stack[DEMO_STACK_SIZE];
static u32									thread_3_stack[DEMO_STACK_SIZE];
static u32									thread_4_stack[DEMO_STACK_SIZE];
static u32									thread_5_stack[DEMO_STACK_SIZE];
static u32									thread_6_stack[DEMO_STACK_SIZE];
static u32									thread_7_stack[DEMO_STACK_SIZE];

#define	TX_NO_TIME_SLICE					100

/* Define what the initial system looks like.  */


/* Define the counters used in the demo application...  */

static ULONG           thread_0_counter;
static ULONG           thread_1_counter;
//static ULONG           thread_1_messages_sent;
static ULONG           thread_2_counter;
//static ULONG           thread_2_messages_received;
static ULONG           thread_3_counter;
static ULONG           thread_4_counter;
static ULONG           thread_5_counter;
static ULONG           thread_6_counter;
static ULONG           thread_7_counter;


/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);
//void    thread_1_entry(ULONG thread_input);
//void    thread_2_entry(ULONG thread_input);
void    thread_3_and_4_entry(ULONG thread_input);
void    thread_5_entry(ULONG thread_input);
void    thread_6_and_7_entry(ULONG thread_input);


#define TIMER_SIG  						0x00000001
#define EVENT_SIG						0x00000002


void    task_init()
{
    /* Create the main thread.  */
	os_create_task(&thread_0, thread_0_stack, DEMO_STACK_SIZE, 1, TX_NO_TIME_SLICE, thread_0_entry, 0, "thread 0");

    /* Create threads 1 and 2. These threads pass information through a ThreadX 
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
#if 0       
    os_create_task(&thread_1, thread_1_stack, DEMO_STACK_SIZE, 16, TX_NO_TIME_SLICE, thread_1_entry, 0, "thread 1");
	os_create_task(&thread_2, thread_2_stack, DEMO_STACK_SIZE, 16, TX_NO_TIME_SLICE, thread_2_entry, 0, "thread 2");
#endif	
	
    /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.  
       An interesting thing here is that both threads share the same instruction area.  */
    os_create_task(&thread_3, thread_3_stack, DEMO_STACK_SIZE, 8, TX_NO_TIME_SLICE, thread_3_and_4_entry, 3, "thread 3");
	os_create_task(&thread_4, thread_4_stack, DEMO_STACK_SIZE, 8, TX_NO_TIME_SLICE, thread_3_and_4_entry, 4, "thread 4");

    /* Create thread 5.  This thread simply pends on an event flag which will be set
       by thread_0.  */
	os_create_task(&thread_5, thread_5_stack, DEMO_STACK_SIZE, 4, TX_NO_TIME_SLICE, thread_5_entry, 5, "thread 5");

    /* Create threads 6 and 7.  These threads compete for a ThreadX mutex.  */
	os_create_task(&thread_6, thread_6_stack, DEMO_STACK_SIZE, 8, TX_NO_TIME_SLICE, thread_6_and_7_entry, 6, "thread 6");
	os_create_task(&thread_7, thread_7_stack, DEMO_STACK_SIZE, 8, TX_NO_TIME_SLICE, thread_6_and_7_entry, 7, "thread 7");

	/* Create the semaphore used by threads 3 and 4.  */
	os_sem_init(&semaphore_0, 1);

	/* Create the mutex used by thread 6 and 7 without priority inheritance.  */
	os_mutex_init(&mutex_0);


	/* Create the message queue shared by threads 1 and 2.  */
//    tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    /* Create the event flags group used by threads 1 and 5.  */
//    tx_event_flags_create(&event_flags_0, "event flags 0");

}



/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{

    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {
        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Print results.  */
        printf("**** ThreadX Demonstration **** (c) 1996-2016 Express Logic, Inc.\n\n");
        printf("           thread 0 events sent:          %lu\n", thread_0_counter);
        printf("           thread 1 messages sent:        %lu\n", thread_1_counter);
        printf("           thread 2 messages received:    %lu\n", thread_2_counter);
        printf("           thread 3 obtained semaphore:   %lu\n", thread_3_counter);
        printf("           thread 4 obtained semaphore:   %lu\n", thread_4_counter);
        printf("           thread 5 events received:      %lu\n", thread_5_counter);
        printf("           thread 6 mutex obtained:       %lu\n", thread_6_counter);
        printf("           thread 7 mutex obtained:       %lu\n\n", thread_7_counter);
        /* Sleep for 10 ticks.  */
		os_delay(100000);

        /* Set event flag 0 to wakeup thread 5.  */
		os_set_sigs(&thread_5, EVENT_SIG);
    }
}

#if 0
void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Send message to queue 0.  */
        status =  tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        /* Increment the message sent.  */
        thread_1_messages_sent++;
    }
}


void    thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we 
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;
        
        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
    }
}
#endif

void    thread_3_and_4_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 3)
            thread_3_counter++;
        else
            thread_4_counter++;

        /* Get the semaphore with suspension.  */
//        status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);
		status = os_sem_wait(&semaphore_0);
		if (status >=0) {

	        /* Sleep for 2 ticks to hold the semaphore.  */
//  	      tx_thread_sleep(2);
			os_delay(2000);

        	/* Release the semaphore.  */
//        	status =  tx_semaphore_put(&semaphore_0);
			os_sem_post(&semaphore_0);
		}
    }
}


void    thread_5_entry(ULONG thread_input)
{
    /* This thread simply waits for an event in a forever loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Wait for event flag 0.  */
		os_wait_sigs(EVENT_SIG);
		os_clr_sigs(&thread_5, EVENT_SIG);
    }
}


void    thread_6_and_7_entry(ULONG thread_input)
{
    /* This function is executed from thread 6 and thread 7.  As the loop
       below shows, these function compete for ownership of mutex_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 6)
            thread_6_counter++;
        else
            thread_7_counter++;

        /* Get the mutex with suspension.  */
        //status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
        os_mutex_wait(&mutex_0);

#if 0
        /* Get the mutex again with suspension.  This shows
           that an owning thread may retrieve the mutex it
           owns multiple times.  */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
#endif
        /* Sleep for 2 ticks to hold the mutex.  */
//        tx_thread_sleep(2);
		os_delay(2000);

        /* Release the mutex.  */
//        status =  tx_mutex_put(&mutex_0);
		os_mutex_post(&mutex_0);

#if 0
        /* Release the mutex again.  This will actually 
           release ownership since it was obtained twice.  */
        status =  tx_mutex_put(&mutex_0);
#endif
    }
}
