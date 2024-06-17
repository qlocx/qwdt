/*********************************************************************************************** 
 *
 * rs485.h 			                	                                                    
 * 				      						                                             	
 * Functional level 1										                				
 * 												                                        	
 * Prepared: Dirk Handzic									                					
 * 												                                        	
 *  												                                        	
 * Rev	Date	Author	Ext int. (Y/N) 	Reason/description	
 * 0.1  240527  DH      Y               Initial version
 *                                                                                           	
 *RS485 driver
 *                                                       
 ***********************************************************************************************/

#ifndef rs485_defined
#define rs485_defined

#define USARTOSC  3000000

enum BUS_STATE{BUS_IDLE, BUS_ADR, BUS_LEN_MSB, BUS_LEN_LSB, BUS_CHK1MSB, BUS_CHK1LSB, BUS_STX, BUS_SRC, BUS_DATA, BUS_CHK, BUS_PROC, BUS_INVALID, BUS_REPLY};


void  rs485_Init(void);
enum BUS_STATE rs485_GetState(void);
void  rs485_TimeHandler(void);

#endif
