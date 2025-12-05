#ifndef __LED_H 
#define __LED_H

extern uint8_t led_speed;
extern uint8_t led_dir;

// 函数声明
void LED_Init(void);
void Running_LED(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED3_ON(void);
void LED3_OFF(void);
void LED4_ON(void);
void LED4_OFF(void);

#define LED1_PIN    GPIO_Pin_12
#define LED2_PIN    GPIO_Pin_13  
#define LED3_PIN    GPIO_Pin_14
#define LED4_PIN    GPIO_Pin_15
#define LED_PORT    GPIOA

#endif
