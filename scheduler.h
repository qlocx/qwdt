/*********************************************************************************** 
* scheduler.h 
*
* Scheduler for execution of periodical tasks 			                                                          
*										                							  
* Functional level 1																  
*										                							    
* Responsible: Dirk Handzic															  
*											 										  						 										  
* 																					  
* Rev	Date	Author	Ext int. (Y/N) 	Reason/description	
* 0.1   200709  DH	    Y				Initial version						  
*  
************************************************************************************/

#ifndef scheduler_def
#define scheduler_def

void    scheduler_Init(void);

void    scheduler_Reboot(void);

void	scheduler_Switch(void);

#endif
