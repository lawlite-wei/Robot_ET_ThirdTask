#include "stm32f10x.h"                  // Device header
#include "PWMServo.h"

void Servo_Init(void)
{
	PWMServo_Init();
}

void Servo_SetAngle(float Angle)
{
	Servo_Angle(Angle / 180 * 2000 + 500);
}
