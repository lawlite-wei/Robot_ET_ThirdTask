#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "HCSRO4.h"
#include "PWM.h"
#include "Motor.h"
#include "Key.h"
#include "Timer.h"
#include "Servo.h"
#include "PWMServo.h"
#include "MPU6050.h"
#include "MyI2C.h"
#include "TCS34725_IIC.h"
#include "TCS34725.h"
#include "OA.h"
#include <math.h>

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
int16_t AX, AY, AZ, GX, GY, GZ;

float Yaw = 0.0f;				//偏航角	

float Yaw_Result;

int main(void)
{
	Key_Init();
	LED_Init();
	OLED_Init();
	Servo_Init();
	HCSRO4_Init();
	Timer_Init();
	PWM_Init();
	Motor_Init();
	MPU6050_Init();
	
	TCS34725_GPIO_Init();//颜色传感器GPIO初始化
	TCS34725_Init();//颜色传感器初始化
	integrationTime(33);
	
	OLED_Clear();
	
	while (1)
	{
		rgb=TCS34725_Get_RGBData();
		RGB888=TCS34725_GetRGB888(rgb);//将原始数据转化为RGB888格式
		RGB565=TCS34725_GetRGB565(rgb);//将原始数据转化为RGB565格式
		Dis_Temp();//转化为可读颜色数据
		
		OLED_ShowFloatNum(0,0,Yaw,3,4,OLED_8X16);
		OLED_ShowString(0,16,"R:",OLED_8X16);
		OLED_ShowString(0,32,"G:",OLED_8X16);
		OLED_ShowString(0,48,"B:",OLED_8X16);
		OLED_ShowNum(16,16,R_Dat,3,OLED_8X16);
		OLED_ShowNum(16,32,G_Dat,3,OLED_8X16);
		OLED_ShowNum(16,48,B_Dat,3,OLED_8X16);
		OLED_Update();
		
		
		if(Key_Check(KEY_3,KEY_DOWN ))
		{
			Startflag = !Startflag;
		}
		
		if(Startflag == 1)
		{
			OA_Control();
			LED_Control();
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
		
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
		
		 if (AX < -32767 || AX > 32767 || AY < -32767 || AY > 32767 || AZ < -32767 || AZ > 32767 ||
            GX < -32767 || GX > 32767 || GY < -32767 || GY > 32767 || GZ < -32767 || GZ > 32767)
        {
            // 数据溢出，跳过本次解算，避免崩溃
            return;
        }
		
		GX += 56;
		GY += 19;
		GZ += 10;
	
		//进一步修正零飘
		if (-1 < GX && GX < 2)GX = 0;
		if (-1 < GY && GY < 2)GY = 0;
		if (-1 < GZ && GZ < 2)GZ = 0;
		
		// 偏航角：仅陀螺仪积分（无加速度计校准，会漂移）
		Yaw += GZ / 32768.0 * 2600 * 0.001;
		
		//输出进一步处理
		if (fabs(Yaw_Result - Yaw) > 0.08)Yaw_Result = Yaw;
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
