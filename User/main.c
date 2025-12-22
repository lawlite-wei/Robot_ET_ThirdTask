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
float last_Yaw;

// 添加转向状态标志
uint8_t turning_flag = 0;       // 0:未转向，1:正在左转，2:正在右转
float target_Yaw = 0.0f;        // 目标偏航角

void turnleft(void);
void turnright(void);
void OA_Control(void);
float normalize_angle(float angle);  // 角度归一化函数

// 角度归一化到0-360度
float normalize_angle(float angle)
{
    angle = fmodf(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    return angle;
}

// 计算角度差（考虑360度边界）
float angle_difference(float current, float target)
{
    float diff = target - current;
    if (diff > 180.0f) diff -= 360.0f;
    else if (diff < -180.0f) diff += 360.0f;
    return diff;
}

void turnleft(void)
{
    if (turning_flag == 0)  // 开始新的转向
    {
        last_Yaw = normalize_angle(Yaw);
        target_Yaw = normalize_angle(last_Yaw + 95.0f);  // 左转90度
        turning_flag = 1;  // 设置为左转状态
    }
    
    // 计算当前角度差
    float current_Yaw = normalize_angle(Yaw);
    float diff = angle_difference(current_Yaw, target_Yaw);
    
    if (fabs(diff) > 2.0f)  // 还未到达目标，继续转向
    {
        // 根据剩余角度调整速度（接近目标时减速）
        float speed = 87.0f;
        if (fabs(diff) < 30.0f) speed = 50.0f;  // 接近目标时减速
        if (fabs(diff) < 10.0f) speed = 30.0f;
        
        Motor_left(-speed);
        Motor_right(speed);
    }
    else  // 到达目标，停止转向
    {
        Motor_left(0);
        Motor_right(0);
        turning_flag = 0;  // 清除转向标志
        Delay_ms(100);     // 停止后短暂延时稳定
    }
}

void turnright(void)
{
    if (turning_flag == 0)  // 开始新的转向
    {
        last_Yaw = normalize_angle(Yaw);
        target_Yaw = normalize_angle(last_Yaw - 95.0f);  // 右转90度
        turning_flag = 2;  // 设置为右转状态
    }
    
    // 计算当前角度差
    float current_Yaw = normalize_angle(Yaw);
    float diff = angle_difference(current_Yaw, target_Yaw);
    
    if (fabs(diff) > 2.0f)  // 还未到达目标，继续转向
    {
        // 根据剩余角度调整速度（接近目标时减速）
        float speed = 87.0f;
        if (fabs(diff) < 30.0f) speed = 50.0f;  // 接近目标时减速
        if (fabs(diff) < 10.0f) speed = 30.0f;
        
        Motor_left(speed);
        Motor_right(-speed);
    }
    else  // 到达目标，停止转向
    {
        Motor_left(0);
        Motor_right(0);
        turning_flag = 0;  // 清除转向标志
        Delay_ms(100);     // 停止后短暂延时稳定
    }
}

void OA_Control(void)
{
    // 如果正在转向，优先完成转向
    if (turning_flag != 0)
    {
        if (turning_flag == 1) turnleft();
        else if (turning_flag == 2) turnright();
        return;
    }
    
    distance_M = HCSRO4_GetValue();
    if(distance_M > 0 && distance_M <= 10)
    {
        Motor_left(0);
        Motor_right(0);
        Delay_ms(100);  // 停止后短暂延时
        HCSRO4_getData();
        
        //左拐
        if(distance_L > 20 && distance_R < 20)
        {
            turnleft();
            Delay_ms(100);
            distance_M = HCSRO4_GetValue();
        }
        //右拐
        else if(distance_R > 20 && distance_L < 20)
        {
            turnright();
            Delay_ms(100);
            distance_M = HCSRO4_GetValue();
        }
        //十字口判断
        else if(distance_L > 20 && distance_R > 20)
        {
            if(distance_L > distance_R)
            {
                turnleft();
            }
            else if(distance_L < distance_R)
            {
                turnright();
            }
            Delay_ms(100);
            distance_M = HCSRO4_GetValue();
        }
        //后退
        else if(distance_L < 20 && distance_R < 20)
        {
            Motor_left(-70);
            Motor_right(-70);
            Delay_ms(925);
            Motor_left(0);
            Motor_right(0);
            Delay_ms(100);
            
            HCSRO4_getData();
        }
    }
    else if (turning_flag == 0)  // 没有障碍且不在转向时才前进
    {
        Motor_left(70);
        Motor_right(70);
    }
}

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
    
    RGB rgb;
    TCS34725_GPIO_Init();//颜色传感器GPIO初始化
    TCS34725_Init();//颜色传感器初始化
    integrationTime(33);
    OLED_Clear();
    
    // 添加Yaw角初始化
    Yaw = 0.0f;
    Yaw_Result = 0.0f;
    last_Yaw = 0.0f;
    turning_flag = 0;
    
    while (1)
    {
        OLED_ShowFloatNum(0,0,Yaw,3,4,OLED_8X16);
        OLED_ShowFloatNum(0,2,normalize_angle(Yaw),3,4,OLED_8X16);  // 显示归一化角度
        OLED_Update();
        
        rgb=TCS34725_Get_RGBData();
        RGB888=TCS34725_GetRGB888(rgb);//将原始数据转化为RGB888格式
        RGB565=TCS34725_GetRGB565(rgb);//将原始数据转化为RGB565格式
        Dis_Temp();//转化为可读颜色数据
        
        if(Key_Check(KEY_3,KEY_DOWN ))
        {
            Startflag = !Startflag;
            if (Startflag == 0)  // 停止时重置转向状态
            {
                turning_flag = 0;
                Motor_left(0);
                Motor_right(0);
            }
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
        
        // 修正数据溢出检查逻辑
        if (AX == -32768 || AX == 32767 || AY == -32768 || AY == 32767 || AZ == -32768 || AZ == 32767 ||
            GX == -32768 || GX == 32767 || GY == -32768 || GY == 32767 || GZ == -32768 || GZ == 32767)
        {
            // 数据溢出，跳过本次解算
            TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
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
        
        // 限制Yaw角范围，避免过大
        if (Yaw > 10000.0f) Yaw -= 360.0f;
        if (Yaw < -10000.0f) Yaw += 360.0f;
        
        //输出进一步处理
        if (fabs(Yaw_Result - Yaw) > 0.08)Yaw_Result = Yaw;
        
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
