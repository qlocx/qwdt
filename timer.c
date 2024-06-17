/*********************************************************************************** 
* timer.c                                                                         
*                                                                                     
* Functional level 1                                                                  
*                                                                                       
* Responsible: Dirk Handzic                                                            
*                                                     
*                                                                                     
* Rev   Date    Author  Ext int. (Y/N)  Reason/description
* 0.1   240507  DH      Y               Initial version                       
*
* Timer handling. Keeping track of system time. Providing slow timers 
* to be used by other functions 
************************************************************************************/

#include    <stm32c011xx.h>

#include    "csetup.h"
#include    "timer.h"



//********************************* Definitions *************************************
       
static UINT timer[NROF_TIMERS];

static uint32_t sys_tick;   
static uint32_t timer_sys;
static uint8_t tick;
static uint8_t last_tick;
//********************************* Internal functions ******************************
void SysTick_Handler(void)
{
    sys_tick++;
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{ 

    if(TIM1->SR & 0x0001){
        TIM1->SR &= 0xFFFE;
        timer_sys++;
        if(tick < 199){
            tick++;
        }
        else{
            tick = 1;
        }                           
    }   
}
//********************************* External functions ******************************


// Decrement slow timers
// executed at 100 ms intervals 
void    timer_DecrementTimers(void)
{
uint8_t index;
    
    for (index = 0;index < NROF_TIMERS; index++){
        if (timer[index]) {
            timer[index]--; 
        } 
    }
}   



void    timer_SetTimer(uint8_t timer_no, UINT time)
{
    if (timer_no < NROF_TIMERS){
        timer[timer_no] = time;
    }
}

UINT   timer_GetTimer(uint8_t timer_no)
{
    if (timer_no < NROF_TIMERS){
        return(timer[timer_no]);
    } else {
        return (0);
    } 
}

// Init TIM1 and TIM3
void timer_Init(void)
{
 uint8_t timer_no;    
    
 	TIM1->ARR = 500;                            // Auto reload 500; 100 kHz /500 = 200 Hz
	TIM1->PSC = (FOSC / (100000)) - 1;          // Timer clock = 100kHz
	TIM1->CR1 = 0x0011;							// Down counter; enable counter
	 
        NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,3);
        NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
	
	TIM1->DIER |= 0x0001; 
	
	for(timer_no = 0;timer_no < NROF_TIMERS;timer_no++){
        timer[timer_no] = 0;
    }  
    timer_sys = 0;
    tick = 0;	
    sys_tick = 0;
    SysTick_Config(FOSC/1000);                  // 1000 us tick
}

// Task handling returns ID for task to be executed
//
uint8_t	timer_Tick(void)
{
	if ( last_tick != tick){
		last_tick = tick;
		return(last_tick);
	}
	else{
		return(0);
	}
}

uint32_t   timer_GetSystemTime(void)
{
    return(timer_sys);
}

uint32_t   timer_GetSysTick(void)
{
    return(sys_tick);
}

