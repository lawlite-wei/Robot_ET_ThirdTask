#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "HCSRO4.h"
#include "PWM.h"
#include "Motor.h"
#include "PWM2.h"
#include "Motor2.h"
#include "Key.h"
#include "Timer.h"

/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0             X轴           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */

uint16_t distance;
uint8_t Startflag = 0;

int main(void)
{
	OLED_Init();
	Key_Init();
	HCSRO4_Init();
	Timer_Init();
	PWM_Init();
	Motor_Init();
	PWM2_Init();
	Motor2_Init();
	
	
	while (1)
	{
//		distance = HCSRO4_GetValue();
//		
//		OLED_Clear();
//		OLED_ShowNum(0,0,distance,3,OLED_8X16);
//		OLED_Update();
		
		
		if(Key_Check(KEY_3,KEY_DOWN ))
		{
			Startflag = !Startflag;
		}
		
		if(Startflag == 1)
		{
			Motor_right(70);
			Motor_left(70);
		}
		else
		{
			Motor_right(0);
			Motor_left(0);
		}
	}
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
