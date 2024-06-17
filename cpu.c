/******************************************************************************************* 
* cpu.c                                                                                     
*                                                                                         
* Functional level 1                                                                 
*                                                                                     
* Prepared: Dirk Handzic                                                             
*                                                                                                                                                         
*                                                                                     
* Rev       Date        Author        Ext int. (Y/N)      Reason/description
* 0.1       240527      DH            Y                   Initial version
*                                                                                                       
* Initialisation of control registers and I/O ports                                         
*******************************************************************************************/
#include <stm32c011xx.h>
#include "csetup.h"
#include "cpu.h"


void cpu_Init(void)
{
    // ***** Configure the system clocks *****    
    // Configure APB2 clock
    
    // Configure APB1 clock
    SystemInit();                                   // system_stm32c011xx.c    

   
	
    // Select HSI48 as clock source
    RCC->CR |= RCC_CR_HSION;                        //12MHZ HSISYS

    // Enable Prefetch Buffer and set Flash Latency
    FLASH->ACR &= (uint32_t)~FLASH_ACR_LATENCY_Msk;  
    FLASH->ACR |= FLASH_ACR_PRFTEN;

    while((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != 0x00);
    while((FLASH->SR & FLASH_SR_BSY1) != 0);
    // HCLK = SYSCLK/4 
    RCC->CFGR |= (uint32_t)(0x09 << RCC_CFGR_HPRE_Pos);   //3MHZ 
    RCC-> CCIPR |= (uint32_t)(0x01 << RCC_CCIPR_USART1SEL_Pos);   //SYSCLK for UART1    	  
    // Config GPIO clock
    
    RCC->IOPENR = RCC_IOPENR_GPIOAEN |
                  RCC_IOPENR_GPIOBEN |
                  RCC_IOPENR_GPIOCEN ;        
                 
    // Config APB1 clock
    RCC->APBENR2 = RCC_APBENR2_USART1EN |
                   RCC_APBENR2_TIM1EN;
    

               
                 
    DBG->APBFZ1 |= DBG_APB_FZ1_DBG_IWDG_STOP | DBG_APB_FZ1_DBG_IWDG_STOP;                             // Stop IWDG and I2C1 at debug

   
    #ifdef WDGEN
    // Enable IWDG
    IWDG->KR = IWDG_WRITE_ACCESS_ENABLE;
    IWDG->PR = 0x00000003;                        // devider/32
    IWDG->RLR = 0x00000FFF;                       // 4 s timeout
    IWDG->KR = IWDG_RELOAD;
    IWDG->KR = IWDG_ENABLE; 
    #endif
}

//Initialise ports
void cpu_IOInit (void)
{ 
    // ***** Configure  GPIO ports A-C *****
    
    // Configure port A
    // Mode    
    GPIOA->MODER    =   (ALTFUN << MODER0) |
                        (ALTFUN << MODER1) |
                        (ALTFUN << MODER2) |
                        (ALTFUN << MODER3) |
                        (OUTPUT << MODER4) |
                        (OUTPUT << MODER5) |
                        (OUTPUT << MODER6) |
                        (OUTPUT << MODER7) |
                        (OUTPUT << MODER8) |
                        (OUTPUT << MODER9) |
                        (OUTPUT << MODER10) |
                        (OUTPUT << MODER11) |
                        (OUTPUT << MODER12) |
                        (ALTFUN << MODER13) |
                        (ALTFUN << MODER14) |
                        (OUTPUT << MODER15);
    //Output type
    GPIOA->OTYPER   =   (PUPL << 0) |
                        (PUPL << 1) |
                        (PUPL << 2) |
                        (PUPL << 3) | 
                        (PUPL << 4) |
                        (PUPL << 5) |
                        (PUPL << 6) |
                        (PUPL << 7) | 
                        (PUPL << 8) |
                        (PUPL << 9) |
                        (PUPL << 10) |
                        (PUPL << 11) | 
                        (PUPL << 12) |
                        (PUPL << 13) |
                        (PUPL << 14) |
                        (PUPL << 15);  
    //Output speed                    
    GPIOA->OSPEEDR  =   (SPD002 << 0) |
                        (SPD050 << 2) |
                        (SPD050 << 4) |
                        (SPD050 << 6) | 
                        (SPD050 << 8) |
                        (SPD050 << 10) |
                        (SPD050 << 12) |
                        (SPD050 << 14) | 
                        (SPD002 << 16) |
                        (SPD002 << 18) |
                        (SPD002 << 20) |
                        (SPD050 << 22) | 
                        (SPD050 << 24) |
                        (SPD050 << 26) |
                        (SPD050 << 28) |
                        (SPD002 << 30);   
    //Pull-up /pull-down
    GPIOA->PUPDR =      (NOPUPD << 0) |
                        (NOPUPD << 2) |
                        (NOPUPD << 4) |
                        (NOPUPD << 6) | 
                        (NOPUPD << 8) |
                        (NOPUPD << 10) |
                        (NOPUPD << 12) |
                        (NOPUPD << 14) | 
                        (NOPUPD << 16) |
                        (NOPUPD << 18) |
                        (NOPUPD << 20) |
                        (NOPUPD << 22) | 
                        (PU << 24) |
                        (NOPUPD << 26) |
                        (NOPUPD << 28) |
                        (NOPUPD << 30); 
    
    // AFR settings
    GPIOA->AFR[0] = (AF4 << AF_PIN0) |          // USART1
                    (AF4 << AF_PIN1) | 
		    (AF1 << AF_PIN2) |          //USART2
                    (AF1 << AF_PIN3) ;				  
                    
             
    GPIOA->AFR[1] = (AF0 << AF_PIN13) |         // SWD
                    (AF0 << AF_PIN14);

   
  
}   

