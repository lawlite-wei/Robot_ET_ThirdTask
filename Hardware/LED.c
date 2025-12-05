#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"

// 全局变量定义
uint8_t led_speed = 0;
uint8_t led_dir = 0;

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}	

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

void LED3_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void LED3_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void LED4_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void LED4_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void Running_LED(void)
{
    static uint32_t counter = 0;
    static uint8_t current_led = 0;
    
    counter++;
    
    // 根据速度设置计数阈值
    // 由于主循环Delay_ms(10)，计数阈值 = 期望时间(ms) / 10
    uint32_t speed_threshold;
    switch(led_speed) {
        case 0: speed_threshold = 50; break;   // 500ms / 10 = 50
        case 1: speed_threshold = 100; break;  // 1000ms / 10 = 100
        case 2: speed_threshold = 20; break;   // 200ms / 10 = 20
        default: speed_threshold = 50; break;
    }
    
    if(counter >= speed_threshold) {
        counter = 0;
        
        // 关闭所有LED
        GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
        
        if(led_dir == 0) {  // 正向
            current_led = (current_led + 1) % 4;
        } else {  // 反向
            current_led = (current_led == 0) ? 3 : (current_led - 1);
        }
        
        // 点亮当前LED
        switch(current_led) {
            case 0: 
                GPIO_ResetBits(GPIOB, GPIO_Pin_12);
                break;
            case 1: 
                GPIO_ResetBits(GPIOB, GPIO_Pin_13);
                break;
            case 2: 
                GPIO_ResetBits(GPIOB, GPIO_Pin_14);
                break;
            case 3: 
                GPIO_ResetBits(GPIOB, GPIO_Pin_15);
                break;
        }
    }
}
