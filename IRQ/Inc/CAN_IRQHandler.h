#ifndef CAN_IRQHANDLER_H
#define CAN_IRQHANDLER_H

#include "EXTI_IRQHandler.h"
#include "main.h"
#include "can.h"
#include "fsm.h"

/* CAN 报文打包发送接口 */
void CAN_Send_Data(uint32_t id, uint8_t *data);

#endif /* CAN_IRQHANDLER_H */   