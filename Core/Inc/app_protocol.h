#ifndef __APP_PROTOCOL_H
#define __APP_PROTOCOL_H

#include "main.h"

/* 1. 双方约定的 CAN ID 定义 */
#define CAN_ID_BREATHE_CTRL     0x101       // 标准帧：主板 -> 从板（呼吸灯控制）
#define CAN_ID_SLAVE_FEEDBACK   0x201       // 标准帧：从板 -> 主板（100Hz float反馈）
#define CAN_ID_BEEP_TRIGGER     0x301       // 标准帧：CANable -> 主/从板（蜂鸣器触发）
#define CAN_ID_NOISE_STD        0x012       // 标准帧：主板发出的 500Hz 噪声
#define CAN_ID_NOISE_EXT        0x02010101  // 扩展帧：从板发出的 500Hz 噪声

/* 2. 呼吸灯控制数据载荷 (DLC = 3 字节) */
typedef struct __attribute__((packed)) {
    uint8_t  enable;       // 0: 停止, 1: 开启
    uint16_t period_ms;    // 呼吸周期 (毫秒)
} BreatheCtrlPacket_t;

/* 3. 从板反馈给主板的数据载荷 (DLC = 7 字节) */
typedef struct __attribute__((packed)) {
    float    data_value;   // 100Hz 变化的浮点数 (4 字节)
    uint16_t period_log;   // 当前呼吸灯周期 (2 字节)
    uint8_t  state_log;    // 当前开关状态 (1 字节)
} FeedbackPacket_t;

/* 4. 蜂鸣器定次鸣叫数据载荷 (DLC = 1 字节) */
typedef struct __attribute__((packed)) {
    uint8_t  beep_count;   // 响铃次数
} BeepCtrlPacket_t;

#endif /* __APP_PROTOCOL_H */