#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "HCSRO4.h"


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

uint16_t distance;

int main(void)
{
	OLED_Init();
	HCSRO4_Init();
	
	while (1)
	{
		distance = HCSRO4_GetValue();
		
		OLED_Clear();
		OLED_ShowNum(0,0,distance,3,OLED_8X16);
		OLED_Update();
		
	}
}
