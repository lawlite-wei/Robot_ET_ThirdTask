#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "HCSRO4.h"
#include "PWM.h"
#include "Motor.h"
#include "Key.h"
#include "Timer.h"
#include "Servo.h"
#include "PWMServo.h"
#include "TCS34725_IIC.h"
#include "TCS34725.h"
#include "OA.h"

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

uint8_t Startflag = 0;

int main(void)
{
	Key_Init();
	LED_Init();
	Servo_Init();
	HCSRO4_Init();
	Timer_Init();
	PWM_Init();
	Motor_Init();
	
	while (1)
	{
		
		if(Key_Check(KEY_3,KEY_DOWN ))
		{
			Startflag = !Startflag;
		}
		
		if(Startflag == 1)
		{
			OA_Control();
		}
		else
		{
			Motor_left(0);
			Motor_right(0);
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
