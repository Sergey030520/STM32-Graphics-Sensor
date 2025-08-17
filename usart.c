#include "usart.h"
#include "configuration.h"


#define UART1_TX 9
#define UART1_RX 10

USARTx *usart1 = USART1_REG;


void init_uart()
{
    usart1->CR1 = 0;

    gpioa->CRH = ~(GPIO_MASK_PORT_CRH(UART1_TX) | GPIO_MASK_PORT_CRH(UART1_RX));
    // tx
    gpioa->CRH |= GPIO_CRH_CNF_MODE(UART1_TX, GPIO_MODE_SPEED_50MHz,
        GPIO_AF_PUSH_PULL);
   
    // rx
    gpioa->CRH |= GPIO_CRH_CNF_MODE(UART1_TX, GPIO_MODE_INPUT,
        GPIO_INPUT_PULL_UP);

    usart1->BRR = 0x1D4C; // 72MHz

    usart1->CR3 =0; //USART_DMAT | USART_DMAR | USART_EIE;

    // usart1->BRR = 0xEA6;//36MHz
    
    usart1->CR2 = 0;
    
    usart1->CR1 = USART_UE | USART_TE | USART_RE; //| USART_RE; // | USART_TCIE |  | USART_TXEIE;
}


void send(uint8_t byte)
{
    usart1->DR = byte;
    while (!(usart1->SR & USART_SR_TXE))
        ;
}
void send_text(char *buff, int length)
{
    for (int i = 0; i < length; ++i)
    {
        send(buff[i]);
    }
}

void USART1_IRQHandler(void)
{
    if (usart1->SR & (USART_SR_ORE | USART_SR_PE | USART_SR_FE | USART_SR_NE))
    {
        usart1->SR = 0;
    }
}