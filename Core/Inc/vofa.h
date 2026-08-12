#ifndef __VOFA_H
#define __VOFA_H

#include "main.h"

/* 使用 __attribute__((packed)) 强制取消内存对齐，确保字节数绝对是精确的 8 字节！ */
typedef struct __attribute__((packed)) {
    float channel1;      // 4 字节：正弦波数据
    uint8_t tail[4];     // 4 字节：0x00, 0x00, 0x80, 0x7F
} Vofa_Frame_t;

void VOFA_Init(void);
void VOFA_SendTask(void);

#endif