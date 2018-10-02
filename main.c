#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <string.h>
 
#define RX_BUF_SIZE 100
#define RX_BUF_SIZE3 20
volatile char RX_FLAG_END_LINE = 0;
volatile char RXi;
volatile char RXc;
volatile char RX_BUF[RX_BUF_SIZE] = {'\0'};
volatile char RX_FLAG_END_LINE3 = 0;
volatile char RXi3;
volatile char RXc3;
volatile char RX_BUF3[RX_BUF_SIZE3] = {'\0'};
char lattitude[12]={"0000.0000"};
char longtitude[12];

void GPS_GetCoord(const char* str, char* lat, char* lon) 
{
 char* pos1;
 if ((strstr(str, "GPGLL") != NULL)&&(strstr(str, ",A,") != NULL)) 
 {	
	pos1 = strstr(str, "LL,");
	if (pos1 != NULL) 
	{
		strncpy(lat, pos1+3, 9);
	} 
	else 
	{
		strncpy(lat, "0000.0000", 9);
	}
	lat[9] = '\0';
	pos1 = strstr(str, ",N,");
	if (pos1 != NULL) 
	{
		strncpy(lon, pos1+3, 9);
	} 
	else 
	{
		strncpy(lon, "0000.0000", 9);		
	}
	lon[9] = '\0';
 }
}


void googleconvert(void)
{
	int y, l,d;
	for (int i=8, l = 9; (i > 1) && (l>2); i--, l--)
	{
		y = i + 3;
		d = l + 3;
		lattitude[y] = lattitude[i];
		longtitude[d] = longtitude[l];
	}
	lattitude[2] = '%';
	lattitude[3] = '2';
	lattitude[4] = '0';
	longtitude[3] = '%';
	longtitude[4] = '2';
	longtitude[5] = '0';
}

void Delay(void) 
{
 volatile uint32_t i;
 for (i=0; i != 0x3100000; i++);
}

void Delay30s(void) 
{
 volatile uint32_t i;
 for (i=0; i != 0x7735940; i++);
}
 
void clear_RXBuffer(void) 
{
  for (RXi=0; RXi<RX_BUF_SIZE; RXi++)
    RX_BUF[RXi] = '\0';
  RXi = 0;
}

void clear_RXBuffer3(void) 
{
  for (RXi=0; RXi3<RX_BUF_SIZE3; RXi3++)
    RX_BUF3[RXi3] = '\0';
  RXi3 = 0;
}

void send_to_uart(uint8_t data) {
while(!(USART1->SR & USART_SR_TC));
USART1->DR=data; 
}

void send_to_uart2(uint8_t data) {
while(!(USART2->SR & USART_SR_TC)); 
USART2->DR=data; 
}

void send_to_uart3(uint8_t data) {
while(!(USART3->SR & USART_SR_TC)); 
USART3->DR=data; 
}

void send_str(char * string) 
{
 uint8_t i=0;
 while(string[i]) 
 {
  send_to_uart(string[i]);
  i++;
 }
 send_to_uart('\r');
 send_to_uart('\n');
}

void send_str2(char * string) 
{
 uint8_t i=0;
 while(string[i]) 
 {
  send_to_uart2(string[i]);
  i++;
 }
 send_to_uart2('\r');
 send_to_uart2('\n');
}

void send_str3(char * string) 
{
 uint8_t i=0;
 while(string[i]) 
 {
  send_to_uart3(string[i]);
  i++;
 }
 send_to_uart3('\r');
 send_to_uart3('\n');
}

void send_strwrn3(char * string) 
{
 uint8_t i=0;
 while(string[i]) 
 {
  send_to_uart3(string[i]);
  i++;
 }
}

void send_lattitude(char * string) 
{
 uint8_t i=0;
 for(;i<12;i++) 
 {
  send_to_uart3(string[i]);
 }
}

void ring(const char* str)
{
	if((strstr(str,"RING")!=NULL)||(strstr(str,"+CMTI")!=NULL))
	{
		 send_str3("AT+CMGF=1");
	   Delay();
		 if((strstr(lattitude,"0000.0000")==NULL)&&(strstr(longtitude,"0000.0000"))==NULL)
		 {
		   send_str3("AT+CMGS=\"+380972482199\"");
		   Delay();
			 googleconvert();
			 send_strwrn3("https://www.google.com.ua/maps/place/");
			 send_lattitude(lattitude);
			 send_strwrn3("+");
			 send_str3(longtitude);
		   send_to_uart3(0x1a);
		   clear_RXBuffer3();
	   }
  }
}

void usart_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB,ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3,ENABLE);
 
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
 
    GPIO_InitTypeDef GPIO_InitStructure;
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    USART_InitTypeDef USART_InitStructure;
 
    /* USART1 configuration ------------------------------------------------------*/
    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
            the SCLK pin
     */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
    USART_Init(USART1, &USART_InitStructure);
		USART_Init(USART2, &USART_InitStructure);
		USART_Init(USART3, &USART_InitStructure);
 
    USART_Cmd(USART1, ENABLE);
		USART_Cmd(USART2, ENABLE);
		USART_Cmd(USART3, ENABLE);
 
    /* Enable the USART1 Receive interrupt: this interrupt is generated when the
        USART1 receive data register is not empty */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}
 
void USART1_IRQHandler(void)
{
    if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
    {
            RXc = USART_ReceiveData(USART1);
            RX_BUF[RXi] = RXc;
            RXi++;
            if (RXi > RX_BUF_SIZE-1) 
						{
							GPS_GetCoord(RX_BUF,lattitude,longtitude);
              clear_RXBuffer();
            }
            RX_FLAG_END_LINE = 1;
            USART_SendData(USART2, RXc);
     }
}

void USART3_IRQHandler(void)
{
    if ((USART3->SR & USART_FLAG_RXNE) != (u16)RESET)
    {
            RXc3 = USART_ReceiveData(USART3);
            RX_BUF3[RXi3] = RXc3;
            RXi3++;
            if (RXi3 > RX_BUF_SIZE3-1) 
						{
							ring(RX_BUF3);
							clear_RXBuffer3();
						}
            RX_FLAG_END_LINE3 = 1;
            USART_SendData(USART2, RXc);
     }
}

int main(void)
{
  usart_init();
  send_str2("AT");
	while(!(strstr(RX_BUF3,"OK")))
{
send_str3("AT");
Delay();
}
while(1)
	 {
		RX_FLAG_END_LINE=0;
		RX_FLAG_END_LINE3=0;		 
	 }
}
