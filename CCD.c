/***************************************
 * ��Ӧ�ܽţ�SI->PTE24  CLK->PTE25  AD->1P3
 * ʱ�䣺       2017.3.28
 */


#include "include.h"
#define  CCD_UART  UART4
uint16 CCD_Buff[128]={0};
uint8  CCD_Draw_Buff[150];
uint8  CCD_Offset;           //����ѹ
uint16 Max_Value=0,Brightness;

//CCD��ʼ��
void CCD_Init()
{
  gpio_init(CCD_CLK, GPO,0);
  gpio_init(CCD_SI, GPO,0); 
  adc_init (CCD1_ADC);
}
//��ʱ200ns
void  delay200ns()
{
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");//��25�� һ��asm("nop")��8ns;
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
}
//�ع� ��һ��ͼ�񲶻�ʱ����ú���ִ�е�ʱ�����ֵΪ�ع�ʱ��   ��ʱ�ɼ�������һ���ع�ʱ�� 
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
  //�ɼ���һ����
  CCD_Buff[0]=adc_once(CCD1_ADC,ADC_12bit);
  Max_Value=CCD_Buff[0];
  CCD_CLK_LOW;
  //�ɼ���2~128����
  for(i=1; i<128; i++)
  {
          delay200ns();
          CCD_CLK_HIGH; 
          delay200ns();
          CCD_Buff[i]=adc_once(CCD1_ADC,ADC_12bit);
          CCD_CLK_LOW;    
        
          if(Max_Value<CCD_Buff[i])Max_Value=CCD_Buff[i];   //�ҳ������е����ֵ
    
   }
  //���͵�129��clk��Ϊ����
  delay200ns();
  CCD_SI_HIGH; 
  delay200ns();
  CCD_SI_LOW;
}
//����һ������(������ֵ������0-127)
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
    
//    uart_putchar(UART0,0x55);     //Ҫ�������ݱ��뷢������ע�Ͳ��֣�
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