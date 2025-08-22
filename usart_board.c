#include "usart_board.h"
#include "rcc.h"
#include "dma.h"
#include "gpio.h"
#include "memory_map.h"
#include "board_pins.h"


UART_HandleTypeDef uart_handle = {0};

int usart_adapter(const char *data, int length)
{
    send_data_usart(&uart_handle, (uint8_t *)data, (uint16_t)length);
    return 0;
}


void init_uart()
{
    DMA_Config dma_tx_config = {
        .dma = DMA1_REG,
        .channel = CHANNEL_4,
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_DATASIZE_8BIT,
        .periph_size = DMA_DATASIZE_8BIT,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0};

    GPIO_PinConfig_t tx_pin_config = {
        .gpiox = USART1_TX_PORT,
        .pin = USART1_TX_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = 0,
    };
    GPIO_PinConfig_t rx_pin_config = {
        .gpiox = USART1_RX_PORT,
        .pin = USART1_RX_PIN,
        .speed = GPIO_MODE_INPUT,
        .pin_mode = GPIO_INPUT_PUPD,
        .af_remap = 0,
    };
    UART_Config_t uart_config = {
        .usart = (USART_Type *)USART1_REG,
        .baud_rate = UART_BAUDRATE_9600,
        .tx_mode = UART_MODE_DMA,
        .rx_mode = UART_MODE_POLLING,
        .dma_tx = dma_tx_config,
        .dma_rx = (DMA_Config){0},
        .tx_port = &tx_pin_config,
        .rx_port = &rx_pin_config,
    };

    RCC_Frequencies rcc_clocks = {0};
    get_clock_frequencies(&rcc_clocks);

    // Настраиваем UART через локальную конфигурацию
    setup_uart(&uart_config, rcc_clocks.APB2_Freq);

    // Копируем данные в глобальный хендл для использования при передаче
    uart_handle.usart = uart_config.usart;
    uart_handle.tx_mode = uart_config.tx_mode;
    uart_handle.rx_mode = uart_config.rx_mode;
    uart_handle.dma_tx = uart_config.dma_tx;
    uart_handle.dma_rx = uart_config.dma_rx;
}
