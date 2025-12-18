#include "stm32f10x.h"                  // Device header
#include "Servo.h"
#include "HCSRO4.h"
#include "Motor.h"
#include "Delay.h"

uint16_t distance_M;
uint16_t distance_R;
uint16_t distance_L;

void HCSRO4_getData(void)
{
	Servo_SetAngle(90);
	distance_M = HCSRO4_GetValue();
	Delay_ms(500);
	
	Servo_SetAngle(180);
	distance_L = HCSRO4_GetValue();
	Delay_ms(500);
	
	Servo_SetAngle(0);
	distance_R = HCSRO4_GetValue();
	Delay_ms(500);
	
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
		
		if(distance_L >= distance_R)
		{
			Motor_left(-70);
			Motor_right(70);
			Delay_ms(600);
		}
		
		else if(distance_R > distance_L)
		{
			Motor_left(70);
			Motor_right(-70);
			Delay_ms(600);
		}
		
		else if(distance_L <= 6 && distance_R <= 6)
		{
			Motor_left(-70);
			Motor_right(-70);
			Delay_ms(700);
		}
	}
	else{
		Motor_left(70);
		Motor_right(70);	
	}
}
