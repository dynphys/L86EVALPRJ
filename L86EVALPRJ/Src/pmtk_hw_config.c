#include "pmtk_hw_config.h"

uint8_t rx_rmc_ready;

uint8_t rx_buffer_cnt;
uint8_t rx_data_ready;
uint8_t rx_buffer_byte;
uint8_t rx_buffer_array[128];
uint8_t rmc_buffer_array[128];

void UART_Init(void)
{
	rx_buffer_cnt=0;
	rx_data_ready=0;
	rx_buffer_byte=0;
	memset(rx_buffer_array,0x00,sizeof(rx_buffer_array));
	
	HAL_UART_Receive_DMA(&huart1,(uint8_t *)&rx_buffer_byte,1);
}

void UART_Write(uint8_t *_bytes, uint8_t _length)
{
	HAL_UART_Transmit_DMA(&huart1, _bytes, _length);
	rx_data_ready=0;
}

uint8_t UART_Read(uint8_t *_bytes)
{
	uint8_t ret=0;
	uint32_t tickstart=0;
	
	//Timeout!!!
	tickstart=HAL_GetTick();
	while(!rx_data_ready && HAL_GetTick()-tickstart<500); //500ms timeout
	
	if(HAL_GetTick()-tickstart<500)
	{
		memcpy(_bytes,rx_buffer_array,sizeof(rx_buffer_array));
		ret=0;
	}
	else
	{
		ret=1;
	}
	
	return ret;
}

//return 0 for success
uint8_t UART_WaitForAck(void)
{
	uint8_t i=0;
	uint8_t ret=0;
	uint8_t AckFlagStartIndex=0;
	uint8_t AnswerFrame[128];
	
	do
	{
		ret=UART_Read(AnswerFrame);
	}
	while(memcmp(AnswerFrame, "$PMTK001", 8)!=0  && ret==0);
	
	if(ret==0)
	{
		AckFlagStartIndex=0;
		while(AckFlagStartIndex<sizeof(AnswerFrame) && i<2) 
		{
			if(AnswerFrame[AckFlagStartIndex]==',')
			{
				i++;
			}
			
				AckFlagStartIndex++;	
		}
		
		switch(AnswerFrame[AckFlagStartIndex])
		{
			case 0: ret=3; break;
			case 1: ret=2; break;
			case 2: ret=1; break;
			case 3: ret=0; break;
			default: ret=0; break;
		}
	}
	else
	{
		ret=4;
	}
	
	return ret;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t pckt[128];
	
	if(huart==&huart1)
	{
		/* First character of the answer */
		if(rx_buffer_byte == '$')
		{
			rx_buffer_cnt=0;
			memset(rx_buffer_array,0x00,sizeof(rx_buffer_array));
		}
		
		rx_buffer_array[rx_buffer_cnt]=rx_buffer_byte;	

		/* Last character of the answer */
		if(rx_buffer_byte == 0x0A)
		{
			memset(pckt,0,128);
			memcpy(pckt,rx_buffer_array,6);
			
			/* Detect RMC frame */
			if(!strcmp((char *)pckt, "$GNRMC"))
			{
				memcpy(rmc_buffer_array, rx_buffer_array, sizeof(rx_buffer_array));
				rx_rmc_ready=1;
			}
			
			rx_data_ready=1;
		}
		else
		{
			rx_buffer_cnt++;
		}
	}
}
