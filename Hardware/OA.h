#ifndef __OA_H
#define __OA_H

#include "TCS34725.h"

extern RGB rgb;
extern uint16_t distance_M;
extern uint16_t distance_R;
extern uint16_t distance_L;
extern uint8_t turning_flag;  // 添加转向标志

void HCSRO4_getData(void);
void LED_Control(void);

#endif
