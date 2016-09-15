#include "stm32l1xx_hal.h"

#ifndef MISC_H
#define MISC_H
HAL_StatusTypeDef EEPROM_WriteWord(uint32_t address, uint32_t data);
HAL_StatusTypeDef EEPROM_WriteByte(uint32_t address, uint32_t data);
void OnboardLed(uint8_t status);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
#endif

extern uint8_t user_btn_flag;


