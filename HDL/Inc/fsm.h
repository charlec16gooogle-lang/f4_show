#ifndef __FSM_H
#define __FSM_H

#include "main.h"

/* 系统状态的枚举 */
typedef enum {
    MODE_OFF = 0,    // 状态0：全灭
    MODE_WATER,      // 状态1：LED1和2 流水灯
    MODE_BREATH      // 状态2：LED3和4 呼吸灯
} SystemMode_t;

/* 状态机对外提供的接口函数声明 */
void FSM_Init(void);   // 状态机初始化
void FSM_Loop(void);  // 状态机核心轮询函数（放在 main 函数的 while(1) 中）

#endif /* __FSM_H */