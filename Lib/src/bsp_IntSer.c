#include "config.h"

uint8_t adcRunFlag;
void InitGPIO(void)
{
	PULSE_IO = 0;
	InvalidOut();
	ALL_LED_OFF();
	GPIO_SetMode(P0,BIT1,GPIO_PMD_INPUT);
	GPIO_SetMode(P0,BIT4|BIT5|BIT6|BIT7,GPIO_PMD_OUTPUT);
	GPIO_SetMode(P2,BIT2|BIT3|BIT6,GPIO_PMD_OUTPUT);
	GPIO_SetMode(P3,BIT5|BIT4,GPIO_PMD_OUTPUT);
}

void WDT_Init(void)
{
	SYS_UnlockReg();
	WDT_Open(WDT_TIMEOUT_2POW18, 0, TRUE, TRUE);
	WDT_EnableInt();
	NVIC_EnableIRQ(WDT_IRQn);
	SYS_LockReg();
}
void GPABC_IRQHandler(void)
{		
}
void GPDEF_IRQHandler(void)
{
}
void TMR2_IRQHandler(void)
{
}
//*********************************************************************************/
void WDT_IRQHandler(void)
{
	if(WDT_GET_TIMEOUT_INT_FLAG() == 1){
		WDT_CLEAR_TIMEOUT_INT_FLAG();
	}
}
//*********************************************************************************/
void ADC_IRQHandler(void)
{
	while((ADC->ADSR&0x01)!=0)
	{	
		ADC->ADSR |= 0x01;											//清中断标志位
		adcData.adc[adcData.tail++]=ADC->ADDR[adcChannel]&0x0FFF;			//读取ADC数据
		ADC->ADCR |= 0x800;											//ADC采样开始
	}
}
//*********************************************************************************/
void TMR0_IRQHandler(void)
{
	if(TIMER_GetIntFlag(TIMER0) == 1){
		TIMER_ClearIntFlag(TIMER0);			/* Clear Timer3 time-out interrupt flag */
		if(ADC->ADCR & ADC_ADCR_ADEN_Msk){
			adcRunFlag = 0; 
			ADC_STOP_CONV(ADC);
			ADC_POWER_DOWN(ADC);
		}
	}
}



void PulseOut(void){
	uint8_t i;
	PULSE_IO = 1;
	i=2;
	while(i--);		
	PULSE_IO = 0;
}
void StartAdcConvertion(uint8_t channel,uint8_t pulse)
{
//	osSignalClear(CalculateThreadID,ADC_ENABLE_MSG|ADC_OVER_MSG);
	ADC_POWER_ON(ADC);
	memset(&adcData,0,sizeof(ADC_DATA_T));
	if(channel == SIGNAL_CHANNEL){
		adcMaxCounter	 = 230;
	}else {
		adcMaxCounter	 = 10;
	}
//	osSignalSet(CalculateThreadID,ADC_ENABLE_MSG);
	adcChannel = channel;
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	ADC->ADCHER = 0;
	ADC->ADCHER |= (1<<adcChannel);
	
	adcRunFlag |= 1;
	if(pulse){
		PulseOut();
		TIMER_Start(TIMER0);
	}
	ADC_START_CONV(ADC);
	
	while(adcRunFlag&0x01){
		while(ADC->ADSR&0x100){
			adcData.adc[adcData.tail++]=ADC->ADDR[0]&0x0FFF;			//读取ADC数据
			if(adcData.tail>adcMaxCounter){
				ADC_STOP_CONV(ADC);
				adcRunFlag = 0;
			}
		}
	}
}


