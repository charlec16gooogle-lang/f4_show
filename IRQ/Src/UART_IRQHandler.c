#include "UART_IRQHandler.h"
#include "EXTI_IRQHandler.h"


uint8_t rx_buffer[5] = {0}; 

void UART_Start_Recieve(void)
{
    HAL_UART_Receive_IT(&huart1, rx_buffer, 5); 
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        //todo
        if (rx_buffer[0] == 0xFF)
        {
            


        }

     
        HAL_UART_Receive_IT(&huart1, rx_buffer, 5); 
    }
}