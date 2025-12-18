#include "stm32f10x.h"                  // Device header
#include "PWM.h"


void Motor_Init(void)
{
    // 1. 开启GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 2. 初始化右电机方向控制引脚 (PA4, PA5, PA6, PA7)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 初始化左电机方向控制引脚 (PB14, PB15, PB6, PB7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	// 4. 初始化PWM
    PWM_Init();
}

// 设置右电机（使用两个电机）
void Motor_right(int8_t PWM)
{
    if (PWM >= 0)
    {
        // 右电机1正转：PA5=1, PA4=0, PWM输出到PA2
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        PWM_SetCompare3(PWM);
        
        // 右电机2正转：PA6=1, PA7=0, PWM输出到PA3
        GPIO_SetBits(GPIOA, GPIO_Pin_6);
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        PWM_SetCompare4(PWM);
    }
    else
    {
        // 右电机1反转：PA5=0, PA4=1
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        PWM_SetCompare3(-PWM);
        
        // 右电机2反转：PA6=0, PA7=1
        GPIO_ResetBits(GPIOA, GPIO_Pin_6);
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        PWM_SetCompare4(-PWM);
    }
}

// 设置左电机（使用两个电机）
void Motor_left(int8_t PWM)
{
    if (PWM >= 0)
    {
        // 左电机1正转：PB14=1, PB15=0, PWM输出到PA0
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        PWM_SetCompare1(PWM);
        
        // 左电机2正转：PB6=1, PB7=0, PWM输出到PA1
        GPIO_SetBits(GPIOB, GPIO_Pin_6);
        GPIO_ResetBits(GPIOB, GPIO_Pin_7);
        PWM_SetCompare2(PWM);
    }
    else
    {
        // 左电机1反转：PB14=0, PB15=1
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
        PWM_SetCompare1(-PWM);
        
        // 左电机2反转：PB6=0, PB7=1
        GPIO_ResetBits(GPIOB, GPIO_Pin_6);
        GPIO_SetBits(GPIOB, GPIO_Pin_7);
        PWM_SetCompare2(-PWM);
    }
}
