/******************************************************************************************* 
* rs485.c   				                                                                     
*										                                                 
* Functional level 1						                                         
*											                                         
* Prepared: Dirk Handzic					                                         
*											                                         							                                         
* 											                                         
* Rev	Date        Author		Ext int. (Y/N)      Reason/description
* 0.2   240612      DH                  N                   timeout added to rs485_PutByte
* 0.1   240507      DH			Y                   Initial version
*                                                                                                       
* RS485 bus driver                                         
*******************************************************************************************/
#include	<stm32c011xx.h>
#include	"csetup.h"
#include	"rs485.h"
#include        "timer.h"
#include        <string.h>
#include        <stdio.h>
#include        "cpu.h"
#include        "wdt.h"
//*************************** Definitions ***********************************

#define MY_ADR            0xAA
#define Q3IOT             0x01



#define BUSR_BUFFER_SIZE  260
#define BUST_BUFFER_SIZE  260
#define D_BUFFER_SIZE     260

// Commands
#define WDT           0x0C 



// Status reply
#define NO_DATA       0x00
#define DATA_OK       0x01
#define READ_DATA     0x02
#define INVALID       0xAA


static uint8_t io_rbuf[BUSR_BUFFER_SIZE];
static uint16_t io_rbuf_wptr;                    // buffer pointer write to buffer 
static uint16_t io_rbuf_rptr;                    // buffer pointer read to buffer 

static uint8_t io_tbuf[BUST_BUFFER_SIZE];
static uint16_t io_tbuf_wptr;                   // buffer pointer write to buffer 
static uint16_t io_tbuf_rptr;                   // buffer pointer write to buffer 

static BOOL empty_buffer;

static enum BUS_STATE bus_state;
static uint8_t received_byte;
static uint16_t crc;
static uint8_t sum1, sum2;
static uint8_t data_buffer[D_BUFFER_SIZE];
static uint16_t rec_cnt;
static uint16_t packet_length;
static uint16_t snd_ptr;
static uint8_t old_cmd;
static BOOL msg_received;
static  BOOL rs485_init_done = FALSE;

//*************************** Internal functions ****************************
// Interrupt routines

void USART1_IRQHandler(void)
{
    do{
        if(USART1->ISR & USART_ISR_ORE){
            USART1->ICR |= USART_ICR_ORECF;              // overrun error
        }  
        if(USART1->ISR & USART_ISR_FE){
            USART1->ICR |= USART_ICR_FECF;            
        }
        if(USART1->ISR & USART_ISR_NE){                 // noise detection error
            USART1->ICR = USART_ICR_NECF;            
        }
        //Receive interrupt                              
        if(USART1->ISR & USART_ISR_RXNE_RXFNE){
            io_rbuf[io_rbuf_wptr++] =  USART1->RDR;
            if (io_rbuf_wptr == BUSR_BUFFER_SIZE){
                io_rbuf_wptr = 0;
            }
        }    
        // Transmitter empty interrupt
        if((USART1->ISR & USART_ISR_TXFE) != 0){
            if (io_tbuf_rptr != io_tbuf_wptr){                        
                USART1->TDR = io_tbuf[io_tbuf_rptr++];
                if (io_tbuf_rptr == BUST_BUFFER_SIZE){
                    io_tbuf_rptr = 0;
                }                   
            } 
            else{
                empty_buffer = TRUE;
                USART1->CR1 &= ~USART_CR1_TXFEIE;
                USART1->CR1|= USART_CR1_TCIE;
            }    
        }
        // Transmission complete interrupt
        if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC){           
            USART1->CR1  &= ~USART_CR1_TCIE;            
            USART1->ICR = USART_ICR_TCCF;                          
        }
        // WUF interrupt
        if((USART1->ISR & USART_ISR_WUF) != 0){
            USART1->ICR |= USART_ICR_WUCF;
        }
    }while((USART1->ISR & USART_ISR_RXNE_RXFNE) || (USART1->ISR & USART_ISR_TXFE) && (!empty_buffer));  
     
}

uint16_t   rs485_GetRcvCount(void)
{
uint16_t count = 0;


    if(io_rbuf_rptr == io_rbuf_wptr){
        count = 0;
    }
    else if(io_rbuf_rptr < io_rbuf_wptr){
        count = io_rbuf_wptr - io_rbuf_rptr;
    }
    else if(io_rbuf_rptr > io_rbuf_wptr){
        count = io_rbuf_wptr + BUSR_BUFFER_SIZE - io_rbuf_rptr;
    }
    return(count);
}
      
// Sends one byte. If sender is busy it puts the byte in a transmit buffer
void     rs485_PutByte(uint8_t data) 
{
uint16_t newpnt;
uint32_t timeout = 50000;
    
      newpnt = io_tbuf_wptr + 1;
      if(newpnt == BUST_BUFFER_SIZE){
          newpnt = 0;
      }
      do{
          timeout--;
      }while((newpnt == io_tbuf_rptr) && (timeout > 0));                       // Wait for at least one free position 
      if(empty_buffer == TRUE){     
          empty_buffer = FALSE;                     
          USART1->TDR = data;
          USART1->CR1 |= USART_CR1_TXFEIE;                 // Enable interrupt         
      }
      else{
          io_tbuf[io_tbuf_wptr] = data;
          io_tbuf_wptr = newpnt;        
      }
     
}    
      
// Puts a null terminated string in a transmit buffer
void    rs485_PutString( uint8_t *buffer)
{
uint16_t index;
uint16_t length;


    length = strlen(buffer); 
    for (index = 0; index < length; index++) {
        rs485_PutByte(buffer[index]);
    }
    
    USART1->CR1 |= USART_CR1_TXFEIE;                 // Enable interrupt
    
}  

// Puts a null terminated string in a transmit buffer
void    rs485_PutBuffer( uint8_t *buffer, uint8_t len)
{
uint16_t index;
uint16_t length;


    length = len; 
    for (index = 0; index < length; index++) {
        rs485_PutByte(buffer[index]);
    }
    
    USART1->CR1 |= USART_CR1_TXFEIE;                 // Enable interrupt
    
}  

// Puts len characters in a transmit buffer
void    rs485_Write(uint8_t *buffer, uint16_t len)
{
uint16_t index;
     
    for (index = 0; index < len; index++) {
        rs485_PutByte(buffer[index]);
    }
    USART1->CR1 |= USART_CR1_TXFEIE;                 // Enable interrupt
    
}     


// Returns byte from receive buffer, returns 0 if empty  

uint8_t   rs485_GetByte(void)
{
uint8_t data;

    if (io_rbuf_rptr == io_rbuf_wptr) {
        return (0);
    }
    data = io_rbuf[io_rbuf_rptr++];
    if (io_rbuf_rptr == BUSR_BUFFER_SIZE) {
        io_rbuf_rptr = 0;
    }    
    return (data);
}

// Read max len characters into buffer
void    rs485_GetString( uint8_t *buffer, uint16_t len)
{
uint16_t count;
uint16_t i;
    
    count = rs485_GetRcvCount();
    if(count > len){
        count = len;
    }
    for(i = 0; i < count;i++){
        *buffer = io_rbuf[io_rbuf_rptr++];
        if (io_rbuf_rptr == BUSR_BUFFER_SIZE) {
            io_rbuf_rptr = 0;
        } 
    }
}                
    
void    rs485_InitCrc(void)
{

    sum1 = 0;
    sum2 = 0;
}

// Fletcher 16
void    rs485_Crc(uint8_t data)
{
    sum1 = (sum1 + data) % 255;
    sum2 = (sum2 + sum1) % 255;
}

void    rs485_Send(uint8_t data)
{
    data_buffer[snd_ptr++] = data;
    rs485_Crc(data);
}

void    rs485_SendCrc(void)
{
    data_buffer[snd_ptr++] = sum2; 
    data_buffer[snd_ptr++] = sum1;
    sum2 = 0;
    sum1 = 0;
}

void    rs485_FlushBuffer(void)
{
uint16_t i;

    for(i = 0;i < D_BUFFER_SIZE;i++){
        data_buffer[i] = 0;
    }    
}


//*************************** External functions ****************************

void rs485_Init(void)
{
uint32_t usartdiv;

    // Config serial port USART1
    empty_buffer = TRUE;
    io_rbuf_wptr = 0;                
    io_rbuf_rptr = 0;                
    io_tbuf_wptr = 0;
    io_tbuf_rptr = 0;
    sum1 = 0;
    sum2 = 0;

    USART1->CR1 &= ~USART_CR1_UE;
    USART1->CR3 = USART_CR3_EIE | USART_CR3_DEM;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE_RXFNEIE;          
                                                  
    NVIC_SetPriority(USART1_IRQn, 15);
    NVIC_EnableIRQ(USART1_IRQn);              
                                    
    usartdiv = (USARTOSC/250000)*4;
    USART1->BRR = usartdiv;         
    rs485_init_done = TRUE;   
    bus_state = BUS_IDLE;      
}




enum BUS_STATE rs485_GetState(void)
{
    return(bus_state);
}    


//Called every 10 ms from scheduler
void  rs485_TimeHandler(void)
{
uint16_t temp;

uint8_t last_byte;


  if(rs485_init_done){    
    // RX state machine 
    
    while((rs485_GetRcvCount() > 0) || msg_received){        
        switch(bus_state){
            case BUS_IDLE: 
                                                  
                if(rs485_GetByte() == 0x01){                    // Find SOH                                    
                    bus_state = BUS_ADR;
                    rs485_InitCrc();
                    rs485_Crc(0x01); 
                    rs485_FlushBuffer();                   
                }
                msg_received = FALSE;
                break;

            case BUS_ADR:
                last_byte = rs485_GetByte();
                if(last_byte ==  MY_ADR){                     // check if it is to MY_ADR
                    bus_state = BUS_LEN_MSB;
                    rs485_Crc(last_byte);
                }
                else{
                    bus_state = BUS_IDLE;
                }
                break;

             case BUS_LEN_MSB:                                       // get the expected length
                received_byte = rs485_GetByte();                
                packet_length = received_byte;
                rs485_Crc(received_byte);
                bus_state = BUS_LEN_LSB;               
                break;      

            case BUS_LEN_LSB:                                       // get the expected length
                received_byte = rs485_GetByte();                
                packet_length = received_byte;
                rs485_Crc(received_byte);
                bus_state = BUS_CHK1MSB;               
                break;

            case BUS_CHK1MSB:                                   // Checksum ok?
                received_byte = rs485_GetByte();
                crc = received_byte << 8;
                bus_state = BUS_CHK1LSB;
                break;

            case BUS_CHK1LSB:
                received_byte = rs485_GetByte();
                crc |= received_byte;
                temp = (sum2 << 8) | sum1;
                if(temp == crc){
                    bus_state = BUS_STX;
                    rs485_InitCrc();                   
                }
                else{
                    bus_state = BUS_IDLE;
                }
                break;  
             
            case BUS_STX:
                received_byte = rs485_GetByte();
                rs485_Crc(received_byte);
                if(received_byte == 0x02){
                    bus_state = BUS_SRC;
                    rec_cnt = 0;
                }
                else{
                    bus_state = BUS_IDLE;
                }
                break;
             
            case BUS_SRC:                                                   // Check source address
                received_byte = rs485_GetByte();
                rs485_Crc(received_byte);
                if(received_byte == Q3IOT){                                 // We accept commands from Q3IOT only
                    bus_state = BUS_DATA;
                }
                else{
                    bus_state = BUS_IDLE;
                }             
                break;
                            
            case BUS_DATA:
                received_byte = rs485_GetByte();
                if(rec_cnt < (packet_length - 4)){                      // dont do crc over the crc
                    rs485_Crc(received_byte);
                }
                data_buffer[rec_cnt++] = received_byte;
                if((rec_cnt + 2) >= packet_length){                      // STX and SRC ADR are part of data length but not stored in buffer 
                    bus_state = BUS_CHK;
                    msg_received = TRUE;
                }
                break;

            case BUS_CHK:
                crc = (data_buffer[rec_cnt - 2] << 8) | data_buffer[rec_cnt - 1];
                temp = (sum2 << 8) | sum1;
                if( temp == crc){
                    bus_state = BUS_PROC;                                 // Go to reply state 
                    snd_ptr = 0;                   
                }
                else{
                    bus_state = BUS_INVALID;
                }
                break;
                

            case BUS_PROC:
                old_cmd = data_buffer[0];
                switch(data_buffer[0]){
                   

                    case WDT: 
                       if(data_buffer[1] == 0){                                 // Write
                            temp = data_buffer[3] << 8;
                            temp |= data_buffer[2];
                            wdt_Set(temp);
                            rs485_InitCrc();       
                            rs485_Send(0x01);
                            rs485_Send(Q3IOT);
                            rs485_Send(0);
                            rs485_Send(0x06);                                   // length
                            rs485_SendCrc();
                            rs485_Send(0x02);                                   // STX for data
                            rs485_Send( MY_ADR);
                            rs485_Send(old_cmd | 0x80);
                            rs485_Send(DATA_OK);
                            rs485_SendCrc();
                            bus_state = BUS_REPLY;                           
                        }                  
                        break;

                   

                    default:                 
                        rs485_InitCrc();       
                        rs485_Send(0x01);
                        rs485_Send(Q3IOT);
                        rs485_Send(0x06); 
                        rs485_Send(0);                                // length
                        rs485_SendCrc();
                        rs485_Send(0x02);                                  // STX for data
                        rs485_Send( MY_ADR);
                        rs485_Send(old_cmd | 0x80);
                        rs485_Send(INVALID);
                        rs485_SendCrc();
                        bus_state = BUS_REPLY;
                        break;
                }                                                   //End cmd word switch
                
                break;

            case BUS_REPLY:                
                rs485_PutBuffer(data_buffer, snd_ptr);
                snd_ptr = 0;
                bus_state = BUS_IDLE;                
                break;

            case BUS_INVALID:
                rs485_InitCrc();      
                rs485_Send(0x01);
                rs485_Send(Q3IOT);
                rs485_Send(0);
                rs485_Send(0x06);                                 // length
                rs485_SendCrc();
                rs485_Send(0x02);
                rs485_Send( MY_ADR); 
                rs485_Send(old_cmd | 0x80);
                rs485_Send(INVALID);
                rs485_SendCrc();
                bus_state = BUS_REPLY;   
                break;
            

            default:
                bus_state = BUS_IDLE;
                break;

        }
    }
  }
}


