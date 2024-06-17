/***************************************************************************
 * cpu.h                                                               
 *                                           
 * This module contains all operations for cpu port initialisation
 *                                                                                      
 * Functional level: 1                                   
 *                                           
 * Prepared: Dirk Handzic                           
 *                                                                               
 *                                           
 * Rev   Date       Author       Ext int. (Y/N)       Reason/description   
 * 0.1  240507       DH           Y                    Initial version
 *                                           
 ***************************************************************************/

#ifndef CPU_defined
#define CPU_defined

#define IWDG_WRITE_ACCESS_ENABLE 0x5555
#define IWDG_WRITE_ACCESS_DISABLE 0x0000
#define IWDG_ENABLE 0xCCCC
#define IWDG_RELOAD 0xAAAA

#define MODER0  0
#define MODER1  2
#define MODER2  4
#define MODER3  6
#define MODER4  8
#define MODER5  10
#define MODER6  12
#define MODER7  14
#define MODER8  16
#define MODER9  18
#define MODER10 20
#define MODER11 22
#define MODER12 24
#define MODER13 26
#define MODER14 28
#define MODER15 30
//GPIO mode
#define INPUT   0
#define OUTPUT  1
#define ALTFUN  2
#define ANALOG  3
// Output type
#define PUPL    0
#define ODRN    1
// Pull-up/ pull-down
#define NOPUPD  0
#define PU      1
#define PD      2
// Speed values
#define SPD002  0
#define SPD025  1
#define SPD050  2
#define SPD100  3
// AFR value
#define AF0     0       //system/SPI1/USART2
#define AF1     1       //SPI1/I2C1/LPTIM
#define AF2     2       //LPTIM/EVENT/SYS/LPUART1
#define AF3     3       //I2C1/EVENT
#define AF4     4       //I2C1/LPUART1/TIM22
#define AF5     5       //TIM2/21/22
#define AF6     6       //LPUART1
#define AF7     7       //COMP1/2


#define AF_PIN0 0
#define AF_PIN1 4
#define AF_PIN2 8
#define AF_PIN3 12
#define AF_PIN4 16
#define AF_PIN5 20
#define AF_PIN6 24
#define AF_PIN7 28
#define AF_PIN8 0
#define AF_PIN9 4
#define AF_PIN10 8
#define AF_PIN11 12
#define AF_PIN12 16
#define AF_PIN13 20
#define AF_PIN14 24
#define AF_PIN15 28
// Initialise controller core
void    cpu_Init(void);

// Initialize IO registers  
// Can be called regularly to ensure that data in io direction  
// registers is correct at all times  
void    cpu_IOInit (void);  
#endif
