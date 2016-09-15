#include "misc.h"

uint8_t user_btn_flag = 0;

HAL_StatusTypeDef EEPROM_WriteWord(uint32_t address, uint32_t data)
 {
    HAL_StatusTypeDef  status;
    address = address + 0x08080000;
    HAL_FLASHEx_DATAEEPROM_Unlock();
    status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, address, data);
    HAL_FLASHEx_DATAEEPROM_Lock();
    
	 return status;
}
 
HAL_StatusTypeDef EEPROM_WriteByte(uint32_t address, uint32_t data)
 {
    HAL_StatusTypeDef  status;
    address = address + 0x08080000;
    HAL_FLASHEx_DATAEEPROM_Unlock();
    status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_BYTE, address, data);
    HAL_FLASHEx_DATAEEPROM_Lock();
    
	 return status;
}
 
void OnboardLed(uint8_t status)
{
	if(status==1)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
	}
}
 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_13)
	{
		user_btn_flag=1;
	}
}


