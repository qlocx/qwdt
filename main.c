/*********************************************************************************************** 
 *
 * main.c 			                	                                                    
 * 				      						                                             	
 * Functional level 1										                				
 * 												                                        	
 * Prepared: Dirk Handzic									                					
 * 												                                        	
 *  												                                        	
 * Rev	Date	Author	Ext int. (Y/N) 	Reason/description
 * 0.1  240527  DH	Y		Initial version
 *
 **********************************************************************************************/
#include	<stm32c011xx.h>
#include        <string.h>
#include	"csetup.h"
#include	"cpu.h"
#include	"timer.h"
#include        "rs485.h"
#include        "scheduler.h"
#include        "wdt.h"

//******************************** Definitions ******************************* 


#undef		externglobal  

const UCHAR sw_major_rev = 0;
const UCHAR sw_minor_rev = 1;



//**************************** Internal functions ****************************


//**************************** External functions ****************************


int main(void)
{
    
START:	
    // Initialize hardware
    
    cpu_Init();	  
    cpu_IOInit();             	  	
    timer_Init();	


    // Initialize level 2 modules

    scheduler_Init();
    rs485_Init();			 
    wdt_Init();
   
    while (1){      
        scheduler_Switch();          
    }									// End while
}

