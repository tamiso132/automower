#include "myuart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <esp_log.h>

#include "stdlib.h"
#include "stdio.h"

typedef uint32_t TickType_t;
#define portMAX_DELAY (TickType_t)0xffffffffUL

#define BASE_GPIO GPIO_NUM_19

uint8_t myuart_init(uint8_t uart_port, gpio_num_t tx_pin, gpio_num_t rx_pin)
{
    const uint8_t uart_num = uart_port;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    const int uart_buffer_size = 128 * 2;

    uart_driver_install(uart_num, uart_buffer_size,
                        uart_buffer_size, 100, NULL, 0);
    uint32_t baud_rate;
    uart_get_baudrate(uart_num, &baud_rate);

    printf("UART: %ld\n", baud_rate);

    return uart_num;
}

// TODO, there can be numerious read after each other instead of using up a cycle
uint8_t myuart_read_time_slot(uint8_t uart_num)
{
    uint8_t initiate_read[] = {0xFF}; // hold for at least 1 micro second
    char buffer[1];
    uart_write_bytes(uart_num, &initiate_read, 1);           // start bit pulls low for 8
    uart_read_bytes(uart_num, &buffer[0], 1, portMAX_DELAY); // first bit tells
    return buffer[0];
}

void myuart_write_bytes(uint8_t uart_num, const uint8_t *p_byte, size_t length)
{
    uart_write_bytes(uart_num, p_byte, length);
}
