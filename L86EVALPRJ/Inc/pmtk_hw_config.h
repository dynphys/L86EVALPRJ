#include "stm32l1xx_hal.h"

#ifndef PMTK_HW_CONFIG
#define PMTK_HW_CONFIG

#include "main.h"
#include "string.h"

void UART_Init(void);
void UART_Write(uint8_t *_bytes, uint8_t _length);
uint8_t UART_Read(uint8_t *_bytes);
uint8_t UART_WaitForAck(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif

extern uint8_t rx_rmc_ready;

extern uint8_t rx_buffer_cnt;
extern uint8_t rx_data_ready;
extern uint8_t rx_buffer_byte;
extern uint8_t rx_buffer_array[128];
extern uint8_t rmc_buffer_array[128];



