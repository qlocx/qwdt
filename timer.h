/*********************************************************************************** 
* timer.h 				                                                          
*										                							  
* Functional level 1																  
*										                							    
* Responsible: Dirk Handzic															  
* 										  
* 																					  
* Rev	Date	Author	Ext int. (Y/N) 	Reason/description
* 0.1   240509  DH    	Y		Initial version						  
*
* Timer handling. Keeping track of system time. Providing slow timers 
* to be used by other functions 
************************************************************************************/

#ifndef timer_def
#define timer_def

#define TIMESLOTS 200						/* 200 timeslots per second */


// Application defined timers
#define NROF_TIMERS 9

#define LED_TIMER 0



// 100 ms timer < 109.2 min
#define ONE_SEC		10
#define TWO_SEC		20
#define THREE_SEC	30
#define FOUR_SEC	40
#define FIVE_SEC	50 
#define SIX_SEC		60
#define EIGHT_SEC	 80
#define TEN_SEC		100
#define TWELVE_SEC	120
#define SIXTEEN_SEC     160
#define TWENTY_SEC      200
#define THIRTY_SEC      300
#define ONE_MIN		600
#define TWO_MIN         1200
#define FIVE_MIN	3000
#define ELEVEN_MIN	6600
#define TWENTY_MIN	12000
#define TWENTYFOUR_MIN	14400
#define FIFTY_MIN	30000

// System times
#define TWOHUNDRED_MS_SYS 40
#define THREEHUNDRED_MS_SYS 60
#define ONE_SEC_SYS 200
#define TEN_SEC_SYS 2000
#define TWO_MIN_SYS 24000
#define TEN_MIN_SYS 120000
#define TWENTY_MIN_SYS 240000

// Systicks
#define ONE_SEC_SYSTICK 100000      // 10 us steps

void      timer_DecrementTimers(void);

void      timer_Init(void);

void      timer_WDT(void);

uint8_t   timer_Tick(void);

void      timer_SetTimer(uint8_t timer_no, UINT time);

uint16_t  timer_GetTimer(uint8_t timer_no);

void      timer_DecrementSlowTimers(void);

void      timer_SetSlowTimer(uint8_t timer_no, UINT time);

uint16_t  timer_GetSlowTimer(uint8_t timer_no);

uint32_t  timer_GetSystemTime(void);

uint32_t	timer_GetSysTick(void);
#endif
