/******************************************************************************************* 
* wdt.c   				                                                                     
*										                                                 
* Functional level 1						                                         
*											                                         
* Prepared: Dirk Handzic					                                         
*											                                         							                                         
* 											                                         
* Rev	Date	   Author		Ext int. (Y/N)      Reason/description
* 0.2   240612     DH                   N                   reinit timeout after WDT_RESET
* 0.1   240527     DH			Y                   Initial version
*                                                                                                       
* Watchdog functions                                         
*******************************************************************************************/
#include	<stm32c011xx.h>
#include	"csetup.h"
#include	"wdt.h"

//*************************** Definitions ***********************************
#define RESET_DELAY     10
#define DEFAULT_TIMEOUT 30000;    // 3000s = 5 min

static uint16_t timeout;          // in 0.1s steps
static uint16_t delay;  

static enum WDT_STATE wdt_state;
//*************************** Internal functions ****************************


//*************************** External functions ****************************

  void  wdt_Init(void)
  {
      timeout = DEFAULT_TIMEOUT;        
      GPIOA->BSRR = GPIO_BSRR_BS4;           //Turn power on
      wdt_state = WDT_COUNTING;
  }

  void wdt_Set(uint16_t time)
  {
      timeout = time;
      wdt_state = WDT_COUNTING;
  }

  // called every 0.1s  
  void  wdt_TimeHandler(void)
  {
      GPIOA->BSRR = GPIO_BSRR_BS6;
      switch(wdt_state){
          
          case WDT_COUNTING:
            if(timeout > 0){
                timeout--;  
            }
            else{
                wdt_state = WDT_RESET;
            }
            break;

          case WDT_RESET:
            GPIOA->BSRR = GPIO_BSRR_BR4;           //Turn power off
            delay = RESET_DELAY;
            wdt_state = WDT_RESET_DELAY;
            break;

          case WDT_RESET_DELAY:
            if(delay > 0){
                delay--;
            }
            else{
                GPIOA->BSRR = GPIO_BSRR_BS4;           //Turn power on
                wdt_state = WDT_COUNTING;
                timeout = DEFAULT_TIMEOUT;
            }
            break;
            
          default:
            timeout = DEFAULT_TIMEOUT;        
            GPIOA->BSRR = GPIO_BSRR_BS4;           //Turn power on
            wdt_state = WDT_COUNTING;
            break;    

      }
      GPIOA->BSRR = GPIO_BSRR_BR6;

  }