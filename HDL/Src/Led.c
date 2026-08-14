#include "Led.h"

void Led_Water(void)
{

    LED_ON(1);
    HAL_Delay(200);
    LED_OFF(1);
    HAL_Delay(200);
    LED_ON(2);
    HAL_Delay(200);
    LED_OFF(2);
    HAL_Delay(200);
    


}