#include "stm32f10x.h"                  // Device header
#include "Servo.h"
#include "HCSRO4.h"
#include "Motor.h"
#include "Delay.h"
#include "TCS34725.h"
#include "TCS34725_IIC.h"
#include "LED.h"

uint16_t distance_M;
uint16_t distance_R;
uint16_t distance_L;

RGB rgb;

void HCSRO4_getData(void)
{
	Servo_SetAngle(90);
	Delay_ms(500);
	distance_M = HCSRO4_GetValue();
	
	Servo_SetAngle(180);
	Delay_ms(500);
	distance_L = HCSRO4_GetValue();
	
	Servo_SetAngle(0);
	Delay_ms(500);
	distance_R = HCSRO4_GetValue();
	
	Servo_SetAngle(90);
}

void OA_Control(void)
{
	distance_M = HCSRO4_GetValue();
	
	if(distance_M > 0 && distance_M <= 10)
	{
		Motor_left(0);
		Motor_right(0);
		HCSRO4_getData();
		
		//左拐
		if(distance_L > 20 && distance_R < 20)
		{
			Motor_left(-87);
			Motor_right(87);
			Delay_ms(700);
		}
		
		//右拐
		else if(distance_R > 20 && distance_L < 20)
		{
			Motor_left(87);
			Motor_right(-87);
			Delay_ms(700);
		}
		
		//右拐+前进小步
		else if(distance_R > 40 && distance_L >20)
		{
			Motor_left(87);
			Motor_right(-87);
			Delay_ms(700);
			
			Motor_left(70);
			Motor_right(70);
			Delay_ms(800);
			
			Motor_left(0);
			Motor_right(0);
			
			HCSRO4_getData();
		
			if(distance_L >= distance_R)
			{
				Motor_left(-90);
				Motor_right(90);
				Delay_ms(700);
			}
			
			else if(distance_R > distance_L)
			{
				Motor_left(90);
				Motor_right(-90);
				Delay_ms(700);
			}
		}
		
		//后退测距
		else if(distance_L < 20 && distance_R < 20)
		{
			Motor_left(-70);
			Motor_right(-70);
			Delay_ms(950);
			Motor_left(0);
			Motor_right(0);
			
			HCSRO4_getData();
		
			if(distance_L >= distance_R)
			{
				Motor_left(-90);
				Motor_right(90);
				Delay_ms(700);
			}
			
			else if(distance_R > distance_L)
			{
				Motor_left(90);
				Motor_right(-90);
				Delay_ms(700);
			}
		}
	}
	else{
		Motor_left(70);
		Motor_right(70);	
	}
}

void LED_Control(void)
{
	if(R_Dat > 200 && G_Dat < 100 && B_Dat < 100)
	{
		LED2_OFF();
		LED3_ON();
		LED1_OFF();
		LED4_OFF();
	}
	else if(R_Dat < 100 && G_Dat > 200 && B_Dat < 100)
	{
		LED3_OFF();
		LED2_ON();
		LED1_OFF();
		LED4_OFF();
	}
	else if(R_Dat < 100 && G_Dat < 200 && B_Dat > 200)
	{
		LED2_OFF();
		LED4_ON();
		LED1_OFF();
		LED3_OFF();
	}
	else if(R_Dat < 100 && G_Dat < 150 && B_Dat < 150)
	{
		LED2_OFF();
		LED1_ON();
		LED4_OFF();
		LED3_OFF();
	}
	else
	{
		LED2_OFF();
		LED1_OFF();
		LED3_OFF();
		LED4_OFF();
	}
}
