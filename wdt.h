/*********************************************************************************************** 
 *
 * wdt.h 			                	                                                    
 * 				      						                                             	
 * Functional level 1										                				
 * 												                                        	
 * Prepared: Dirk Handzic									                					
 * 												                                        	
 *  												                                        	
 * Rev	Date	Author	Ext int. (Y/N) 	Reason/description	
 * 0.1  240527  DH      Y               Initial version
 *                                                                                           	
 *watchdog driver
 *                                                       
 ***********************************************************************************************/

#ifndef wdt_defined
#define wdt_defined

enum WDT_STATE{WDT_COUNTING, WDT_RESET, WDT_RESET_DELAY};

void wdt_Init(void);
void wdt_Set(uint16_t time);
void wdt_TimeHandler(void);

#endif
