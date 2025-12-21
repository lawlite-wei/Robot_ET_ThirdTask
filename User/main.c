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
#include "MPU6050.h"
#include "MyI2C.h"
#include "TCS34725_IIC.h"
#include "TCS34725.h"
#include <math.h>

// 全局变量定义
uint8_t Startflag = 0;
int16_t AX, AY, AZ, GX, GY, GZ;

float Yaw = 0.0f;
float Yaw_Result;

uint16_t distance_M;
uint16_t distance_R;
uint16_t distance_L;

float last_Yaw = 0.0f;
float target_Yaw = 0.0f;
uint8_t is_turning = 0;
uint32_t turn_timer = 0;
uint8_t avoidance_step = 0;  // 避障步骤

// 函数声明
void HCSRO4_getData(void);
float normalizeAngle(float angle);
float getAngleToTarget(float current, float target);
void turnLeft90(void);
void turnRight90(void);
void checkTurnComplete(void);
void simpleObstacleAvoidance(void);
void OA_Control(void);

// 获取三个方向的距离
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

// 标准化角度到0-360度
float normalizeAngle(float angle)
{
    while (angle >= 360.0f) angle -= 360.0f;
    while (angle < 0.0f) angle += 360.0f;
    return angle;
}

// 计算到目标角度的最小转角（带方向）
float getAngleToTarget(float current, float target)
{
    current = normalizeAngle(current);
    target = normalizeAngle(target);
    
    float diff = target - current;
    
    // 确保是最短路径 [-180, 180]
    if (diff > 180.0f) diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    
    return diff;
}

// 转向特定角度（正值为右转，负值为左转）
void turnAngle(float angle_deg)
{
    if (is_turning) return;
    
    is_turning = 1;
    turn_timer = 0;
    last_Yaw = Yaw;
    target_Yaw = normalizeAngle(Yaw + angle_deg);  // 注意这里：angle_deg为正表示右转，为负表示左转
    
    if (angle_deg > 0) {
        // 右转
        Motor_left(70);
        Motor_right(-70);
    } else {
        // 左转
        Motor_left(-70);
        Motor_right(70);
    }
}

// 左转90度
void turnLeft90(void)
{
    turnAngle(-90.0f);
}

// 右转90度
void turnRight90(void)
{
    turnAngle(90.0f);
}

// 检查是否完成转向
void checkTurnComplete(void)
{
    if (!is_turning) return;
    
    turn_timer++;
    
    // 计算当前需要转的角度（考虑方向）
    float angle_to_go = getAngleToTarget(Yaw, target_Yaw);
    
    // 调试：打印角度信息（如果有串口）
    // printf("Current: %.2f, Target: %.2f, ToGo: %.2f\n", Yaw, target_Yaw, angle_to_go);
    
    // 角度差小于15度或超时1.5秒就停止
    if (fabs(angle_to_go) < 15.0f || turn_timer > 150)
    {
        Motor_left(0);
        Motor_right(0);
        is_turning = 0;
        turn_timer = 0;
        avoidance_step = 0;  // 重置避障步骤
        Delay_ms(200);  // 转向完成后的停顿
    }
}

// 简化避障逻辑 - 修复版本
void simpleObstacleAvoidance(void)
{
    static uint8_t last_action = 0;
    static uint32_t forward_timer = 0;
    
    distance_M = HCSRO4_GetValue();
    
    // 如果没有障碍物，正常前进
    if (distance_M > 20 || distance_M == 0)  // 20cm外或无信号
    {
        Motor_left(70);
        Motor_right(70);
        forward_timer++;
        last_action = 0;
        avoidance_step = 0;
        
        // 每前进一段时间稍微检查一次
        if (forward_timer > 100) {  // 约1秒
            forward_timer = 0;
            HCSRO4_getData();  // 偶尔检查一下左右
        }
        return;
    }
    
    // 检测到障碍物（10-20cm）
    if (distance_M > 10 && distance_M <= 20)
    {
        // 减速
        Motor_left(50);
        Motor_right(50);
        last_action = 0;
        avoidance_step = 0;
        return;
    }
    
    // 很近的障碍物（0-10cm）需要避障
    if (distance_M > 0 && distance_M <= 10)
    {
        // 停止
        Motor_left(0);
        Motor_right(0);
        Delay_ms(300);
        
        // 获取左右距离
        HCSRO4_getData();
        
        // 避免重复执行相同的避障动作
        if (last_action == 1 && avoidance_step > 0) {
            return;
        }
        
        // 决策逻辑
        if (distance_L > 30 && distance_R > 30) {
            // 两边都很开阔，选择左边
            turnLeft90();
            last_action = 1;
            avoidance_step = 1;
        }
        else if (distance_L > 30) {
            // 左边开阔
            turnLeft90();
            last_action = 1;
            avoidance_step = 1;
        }
        else if (distance_R > 30) {
            // 右边开阔
            turnRight90();
            last_action = 2;
            avoidance_step = 1;
        }
        else if (distance_L > distance_R) {
            // 左边比右边开阔
            // 转一个小角度，而不是90度
            if (distance_L - distance_R > 10) {
                turnAngle(-45.0f);  // 左转45度
            } else {
                turnAngle(-30.0f);  // 左转30度
            }
            last_action = 1;
            avoidance_step = 1;
        }
        else {
            // 右边比左边开阔或相等
            if (distance_R - distance_L > 10) {
                turnAngle(45.0f);  // 右转45度
            } else {
                turnAngle(30.0f);  // 右转30度
            }
            last_action = 2;
            avoidance_step = 1;
        }
        
        forward_timer = 0;
    }
}

// 主控制函数
void OA_Control(void)
{
    static uint32_t control_counter = 0;
    control_counter++;
    
    // 每3次循环检查一次转向状态（约30ms）
    if (control_counter % 3 == 0) {
        if (is_turning) {
            checkTurnComplete();
        }
    }
    
    // 每2次循环执行一次避障检测（约20ms）
    if (control_counter % 2 == 0) {
        if (!is_turning) {
            simpleObstacleAvoidance();
        }
    }
    
    // 防止计数器溢出
    if (control_counter > 10000) {
        control_counter = 0;
    }
}

int main(void)
{
    Key_Init();
    LED_Init();
    Servo_Init();
    HCSRO4_Init();
    Timer_Init();
    PWM_Init();
    Motor_Init();
    MPU6050_Init();
    
    // 初始化变量
    Yaw = 0.0f;
    is_turning = 0;
    turn_timer = 0;
    avoidance_step = 0;
    
    Delay_ms(1000);
    
    while (1)
    {
        if (Key_Check(KEY_3, KEY_DOWN))
        {
            Startflag = !Startflag;
            Delay_ms(200);
            
            if (Startflag) {
                // 启动时重置
                Yaw = 0.0f;
                is_turning = 0;
                avoidance_step = 0;
            }
        }
        
        // 紧急停止
        if (Key_Check(KEY_3, KEY_DOWN)) {
            Motor_left(0);
            Motor_right(0);
            Startflag = 0;
            is_turning = 0;
            avoidance_step = 0;
        }
        
        if (Startflag == 1)
        {
            OA_Control();
            Delay_ms(10);
        }
        else
        {
            Motor_left(0);
            Motor_right(0);
            Delay_ms(50);
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
            TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
            return;
        }
        
        // 零偏修正
        GZ += 10;
        if (GZ > -2 && GZ < 2) GZ = 0;
        
        // 积分得到角度（注意积分系数）
        Yaw += (float)GZ / 32768.0f * 2600.0f * 0.001f;
        
        // 保持角度在合理范围内
        if (Yaw > 180.0f) Yaw -= 360.0f;
        if (Yaw < -180.0f) Yaw += 360.0f;
        
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
