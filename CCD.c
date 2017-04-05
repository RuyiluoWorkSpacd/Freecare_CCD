/***************************************
 * 对应管脚：SI->PTE24  CLK->PTE25  AD->1P3
 * 时间：       2017.3.28
 */


#include "include.h"
#define  CCD_UART  UART4
uint16 CCD_Buff[128]={0};
uint8  CCD_Draw_Buff[150];
uint8  CCD_Offset;           //暗电压
uint16 Max_Value=0,Brightness;

//CCD初始化
void CCD_Init()
{
  gpio_init(CCD_CLK, GPO,0);
  gpio_init(CCD_SI, GPO,0); 
  adc_init (CCD1_ADC);
}
//延时200ns
void  delay200ns()
{
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");//共25个 一个asm("nop")是8ns;
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
}
//曝光 下一次图像捕获时刻与该函数执行的时刻与差值为曝光时间   定时采集就有了一个曝光时间 
void CCD_Exposure()    
{
    CCD_SI_HIGH;        
    delay200ns();
    CCD_CLK_HIGH; 
    delay200ns();
    CCD_SI_LOW;    
    delay200ns();
    CCD_CLK_LOW;    
    for(uint8 i=0; i<129; i++)
    {
        delay200ns();
        CCD_CLK_HIGH; 
        delay200ns();
        CCD_CLK_LOW; 
    } 
}

void CCD_Capture()
{
  uint8  i=0; 
  CCD_SI_HIGH;
   delay200ns();
  CCD_CLK_HIGH;  
  delay200ns();
  CCD_SI_LOW;
  //采集第一个点
  CCD_Buff[0]=adc_once(CCD1_ADC,ADC_12bit);
  Max_Value=CCD_Buff[0];
  CCD_CLK_LOW;
  //采集第2~128个点
  for(i=1; i<128; i++)
  {
          delay200ns();
          CCD_CLK_HIGH; 
          delay200ns();
          CCD_Buff[i]=adc_once(CCD1_ADC,ADC_12bit);
          CCD_CLK_LOW;    
        
          if(Max_Value<CCD_Buff[i])Max_Value=CCD_Buff[i];   //找出数据中的最大值
    
   }
  //发送第129个clk作为结束
  delay200ns();
  CCD_SI_HIGH; 
  delay200ns();
  CCD_SI_LOW;
}
//做归一化处理(将采样值限制在0-127)
void CCD_Normalization()
{
  uint8 i=0;
  float ratio;
  int temp;
  if(Max_Value>CCD_Offset)
  ratio=127.0/(Max_Value-CCD_Offset);
  else  ratio=0;
  for(i=0; i<128; i++)
  {
    if(CCD_Buff[i]>CCD_Offset)
    temp=CCD_Buff[i]-CCD_Offset;
    else temp=0;
    CCD_Buff[i]=(int)(temp*ratio);
    if(CCD_Buff[i]>126) CCD_Buff[i]=126;
    CCD_Draw_Buff[i]=CCD_Buff[i];
  }
  CCD_Draw_Buff[130]=BYTE0(Max_Value);
  CCD_Draw_Buff[131]=BYTE1(Max_Value); 
}


void MingYou_Send_CCD(void)
{
    uint8 i,num=140;
    
//    uart_putchar(UART0,0x55);     //要保存数据必须发送它（注释部分）
//    uart_putchar(UART0,0xaa); 
//    uart_putchar(UART0,0xae); 
    
    uart_putchar(CCD_UART,0x55);     
    uart_putchar(CCD_UART,0xaa); 
    uart_putchar(CCD_UART,0xa1); 
    uart_putchar(CCD_UART,num); 
    
    for(i=0;i<num;i++)
    {
        uart_putchar(CCD_UART,CCD_Draw_Buff[i]); 
    }
    uart_putchar(CCD_UART,0x01); 

}