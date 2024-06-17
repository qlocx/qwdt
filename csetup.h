/*********************************************************************************** 
* csetup.h                                                                        
*                                                                                     
* Functional level 1                                                                  
*                                                                                       
* Responsible: Dirk Handzic                                                           
*                                                             
*                                                                                     
* Rev   Date    Author  Ext int. (Y/N)  Reason/description
* 0.1   240527  DH      Y               Initial version                       
*
*
************************************************************************************/

#ifndef csetup_def
#define csetup_def

#define FOSC      3000000
#define APB2_CLK  3000000

//#define DEBUG_MSG                   // Comment this out for standalone operation

//Peripheral bit banding
#define PBB(R,B) *(volatile unsigned long *)(PERIPH_BB_BASE+((&(R)-(volatile uint32_t *)PERIPH_BASE)*4*32)+(B*4))

enum BOOL_TYPE {FALSE,TRUE};
#define BOOL enum BOOL_TYPE

typedef unsigned char  UCHAR;
typedef unsigned short UINT;
typedef unsigned long  ULONG;
typedef signed short INT;
typedef signed char CHAR;
typedef signed long LONG;
typedef float FLOAT;


//Interrupt priority levels

#define USART2_PRIO        3           
#define LPUART_PRIO        4
#define EXTI_PRIO          5
#define GEN_TIM_PRIO      10          // General timer 

//#define TOF               1
#define QDA               1
#define NDEBUG            1
//#define BUZZER            1
#endif


