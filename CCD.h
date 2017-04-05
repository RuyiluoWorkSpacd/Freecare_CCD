#ifndef _CCD_H_
#define _CCD_H_
#include "common.h"

#define  CCD_CLK   PTC3 
#define  CCD_SI    PTC4
#define  CCD1_ADC  ADC1_SE8
#define  CCD2_ADC  ADC0_DM0


#define  CCD_CLK_HIGH gpio_set(CCD_CLK,1)
#define  CCD_CLK_LOW  gpio_set(CCD_CLK,0)

#define  CCD_SI_HIGH  gpio_set(CCD_SI,1)
#define  CCD_SI_LOW   gpio_set(CCD_SI,0)
 

extern uint16 CCD_Buff[128];
extern uint8 CCD_Draw_Buff[150];
extern uint16 Max_Value,Brightness;
extern uint8  CCD_Offset; 
void CCD_Init();
void CCD_Normalization();
void CCD_Capture();
void CCD_Exposure(); 
void Send_Begin();
void MingYou_Send_CCD(void);

#endif