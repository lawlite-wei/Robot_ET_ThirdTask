//#include "stm32f10x.h"                  // Device header
//#include "Servo.h"
//#include "HCSRO4.h"
//#include "Motor.h"
//#include "Delay.h"
//#include "TCS34725.h"
//#include "TCS34725_IIC.h"
//#include "LED.h"
//#include <math.h>
//#include <stdio.h>

//uint16_t distance_M;
//uint16_t distance_R;
//uint16_t distance_L;

//RGB rgb;

////float last_Yaw = 0.0f;
////float target_Yaw = 0.0f;
////extern float Yaw;
////uint8_t is_turning = 0;  // 转向标志位
////uint8_t obstacle_avoidance_mode = 0;  // 避障模式

//float getAngleDifference(float current, float target)
//{
//    float diff = target - current;
//    
//    // 标准化到[-180, 180]度
//    if (diff > 180.0f) diff -= 360.0f;
//    if (diff < -180.0f) diff += 360.0f;
//    
//    return diff;
//}

//// 辅助函数：标准化角度到0-360度
//float normalizeAngle(float angle)
//{
//    while (angle >= 360.0f) angle -= 360.0f;
//    while (angle < 0.0f) angle += 360.0f;
//    return angle;
//}

//void HCSRO4_getData(void)
//{
//	Servo_SetAngle(90);
//	Delay_ms(500);
//	distance_M = HCSRO4_GetValue();
//	
//	Servo_SetAngle(180);
//	Delay_ms(500);
//	distance_L = HCSRO4_GetValue();
//	
//	Servo_SetAngle(0);
//	Delay_ms(500);
//	distance_R = HCSRO4_GetValue();
//	
//	Servo_SetAngle(90);
//}

////不用延时转向，改用陀螺仪进行转向
////void turnLeft90(void)
////{
////    is_turning = 1;
////    last_Yaw = Yaw;
////    target_Yaw = normalizeAngle(Yaw + 90.0f);  // 左转90度
////    
////    // 开始转向
////    Motor_left(-80);  // 稍微降低速度
////    Motor_right(80);
////}

////void turnRight90(void)
////{
////    is_turning = 1;
////    last_Yaw = Yaw;
////    target_Yaw = normalizeAngle(Yaw - 90.0f);  // 右转90度
////    
////    // 开始转向
////    Motor_left(80);
////    Motor_right(-80);
////}

////// 检查是否完成转向
////void checkTurnComplete(void)
////{
////    if (!is_turning) return;
////    
////    float current_angle = normalizeAngle(Yaw);
////    float target_angle = normalizeAngle(target_Yaw);
////    float diff = getAngleDifference(current_angle, target_angle);
////    
////    // 如果角度差小于5度，认为转向完成
////    if (fabs(diff) < 5.0f)
////    {
////        Motor_left(0);
////        Motor_right(0);
////        is_turning = 0;
////        Delay_ms(100);  // 转向完成后的停顿
////    }
////}

////// 简化避障逻辑
////void simpleObstacleAvoidance(void)
////{
////    distance_M = HCSRO4_GetValue();
////    
////    // 检测前方障碍物
////    if (distance_M > 0 && distance_M <= 10)  // 增加到15cm检测
////    {
////        // 停止前进
////        Motor_left(0);
////        Motor_right(0);
////        Delay_ms(200);
////        
////        // 获取左右距离
////        HCSRO4_getData();
////        
////        // 情况1：左边更开阔
////        if (distance_L > 20 && distance_R <= 10)
////        {
////            turnLeft90();
////        }
////        // 情况2：右边更开阔
////        else if (distance_R > 20 && distance_L <= 10)
////        {
////            turnRight90();
////        }
////        // 情况3：两边都开阔，选择更开阔的一边
////        else if (distance_L > 25 && distance_R > 25)
////        {
////            if (distance_L >= distance_R)
////            {
////                turnLeft90();
////            }
////            else
////            {
////                turnRight90();
////            }
////        }
////        // 情况4：两边都窄，后退然后选择
////        else if (distance_L <= 20 && distance_R <= 20)
////        {
////            // 后退
////            Motor_left(-70);
////            Motor_right(-70);
////            Delay_ms(950);
////            Motor_left(0);
////            Motor_right(0);
////            
////            // 重新检测
////            HCSRO4_getData();
////            
////            if (distance_L >= distance_R)
////            {
////                turnLeft90();
////            }
////            else
////            {
////                turnRight90();
////            }
////        }
////        else
////        {
////            // 默认右转
////            turnRight90();
////        }
////        
////        // 等待转向完成
////        while (is_turning)
////        {
////            checkTurnComplete();
////            Delay_ms(10);
////        }
////        
////        // 转向完成后前进一小段
////        Motor_left(70);
////        Motor_right(70);
////        Delay_ms(500);
////    }
////    else
////    {
////        // 没有障碍物，直行
////        if (!is_turning)  // 只有在没有转向时才直行
////        {
////            Motor_left(65);
////            Motor_right(65);
////        }
////    }
////}

////// 不用延时转向，改用陀螺仪进行转向
////void OA_Control(void)
////{

////    // 如果是正在转向，检查转向是否完成
////    if (is_turning)
////    {
////        checkTurnComplete();
////    }
////    else
////    {
////        // 执行避障逻辑
////        simpleObstacleAvoidance();
////    }
////}

//void LED_Control(void)
//{
//	rgb=TCS34725_Get_RGBData();
//	RGB888=TCS34725_GetRGB888(rgb);//将原始数据转化为RGB888格式
//	RGB565=TCS34725_GetRGB565(rgb);//将原始数据转化为RGB565格式
//	Dis_Temp();//转化为可读颜色数据
//	
//	
//	if(R_Dat > 200 && G_Dat < 100 && B_Dat < 100)
//	{
//		LED2_OFF();
//		LED3_ON();
//		LED1_OFF();
//		LED4_OFF();
//	}
//	else if(R_Dat < 100 && G_Dat > 200 && B_Dat < 100)
//	{
//		LED3_OFF();
//		LED2_ON();
//		LED1_OFF();
//		LED4_OFF();
//	}
//	else if(R_Dat < 100 && G_Dat < 200 && B_Dat > 200)
//	{
//		LED2_OFF();
//		LED4_ON();
//		LED1_OFF();
//		LED3_OFF();
//	}
//	else if(R_Dat < 100 && G_Dat < 150 && B_Dat < 150)
//	{
//		LED2_OFF();
//		LED1_ON();
//		LED4_OFF();
//		LED3_OFF();
//	}
//	else
//	{
//		LED2_OFF();
//		LED1_OFF();
//		LED3_OFF();
//		LED4_OFF();
//	}
//}
