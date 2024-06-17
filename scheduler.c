/***************************************************************************
 * scheduler.c
 *
 * Scheduler for execution of periodical tasks
 *
 *
 * Functional level: 1
 *
 * Prepared: Dirk Handzic
 *
 *
 * Rev  Date    Author  Ext int. (Y/N)  Reason/description
 * 0.1  240527  DH      Y               Initial version
 *
 *
 ***************************************************************************/
#include    <stm32c011xx.h>
#include    <stddef.h>
#include    "csetup.h"
#include    "scheduler.h"
#include    "timer.h"
#include    "cpu.h"
#include    "rs485.h"
#include    "wdt.h"


/***************** Definitions *********************************************/

typedef struct{
        UCHAR           period;
        UCHAR           start_tick;
        UCHAR           count_down;
        ULONG           duration;
        UCHAR           id;
		void (*func)(void);
}TASK_STRUCT;






static TASK_STRUCT tasks[] = {      
                                    {2, 2, 255, 0, 2, rs485_TimeHandler},
                                    {20, 3, 255, 0, 1, timer_DecrementTimers}, 
                                    {20, 4, 255, 0, 4, wdt_TimeHandler},                                                                 							
                                    {0, 0, 0, 0, 0, NULL}
                             };

static BOOL reboot;
static ULONG task_tick;
static UCHAR fault_task;
//*************************** Internal functions ****************************

//*************************** External functions ****************************
#pragma switch direct
void    scheduler_Init(void)
{
    reboot = FALSE;
    fault_task = 0;
}

void  scheduler_Reboot(void)
{
    reboot = TRUE;
}

void    scheduler_Switch(void)
{
UCHAR schedule_tick;
TASK_STRUCT *ptr;

    schedule_tick = timer_Tick();
                                                                                                    // call as often as possible
    if(schedule_tick != 0){
        if(!reboot){
            IWDG->KR = IWDG_RELOAD;                                                                 // kick watchdog
        }		
		
        // Check task table and run tasks assigned to the current time slot
        for (ptr = tasks; ptr->period != 0; ptr++){ 
            if(ptr->count_down){
                ptr->count_down--;
            }        
            if((ptr->count_down == 0) || (ptr->start_tick == schedule_tick)){
                ptr->count_down = ptr->period;
                task_tick = timer_GetSysTick();
                (ptr->func)();
                ptr->duration = timer_GetSysTick() - task_tick;
                if(ptr->duration > 50){
                    fault_task = ptr->id;
                }
            }                                                                                       // end if task found 
        }                                                                                           // end for ptr 
    }      
}

