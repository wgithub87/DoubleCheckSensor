#include "config.h"
//***************************************************/


#define MAX_VALUE							350
#define MAX_AVR_VALUE					350

#define MAX_SIGNAL_VALUE			400
#define MEDIUM_SIGNAL_VALUE		320
#define MIN_SIGNAL_VALUE			280

#define	START_I								30
#define	STOP_I								55

typedef struct
{
	uint32_t Noise_avr;
	uint16_t Noise_max;
	uint16_t Noise_prop;
	uint32_t Amp_avr;
	uint32_t Amp_avr_1;
	uint32_t Amp_avr_2;
	uint32_t Amp_time_1;
	uint16_t Amp_time_2;
	
	uint16_t Amp_Max_Value;
	uint16_t Amp_Max_i;
	uint16_t Amp_counter;
	uint32_t Amp_prop;
	uint32_t ampAvr;
	uint8_t	 largeCounter;
	uint8_t	 mediumCounter;
	uint8_t	 smallCounter;
	uint8_t	 i;
}ECHO_PROP_T;
//***************************************************/
ECHO_PROP_T Amp0;

ECHO_PROP_T* Now_amp;





uint16_t noiseStart_i,noiseStop_i,firstSignal_i,secondSignal_i;										//noiseStart_i:30us后理论上为
uint16_t noiseCounter,avrCounter;								//噪声最大值


uint16_t ampMax_i[20],ampMaxValue[20];
uint8_t i_temp[20];							//用于存储上升沿及其下降沿的i值
uint16_t flip_flag;

//***************************************************/
//----------------------------------------------------
void CalculateThread(void const *argument)
{
	gs_out.resultSt = 3;
	while(1){
		osSignalWait(ADC_ENABLE_MSG, osWaitForever);
		
		
		StartAdcConvertion(SIGNAL_CHANNEL,1);
		
		Now_amp = &Amp0;
		gs_out.u8MeasureSt = 0;
		memset(Now_amp,0,sizeof(ECHO_PROP_T));
		noiseStart_i=0;noiseStop_i=0;firstSignal_i=0;secondSignal_i=0;		
		avrCounter=0;
		noiseCounter=0;
		gs_out.measureSt = 0;
		/****滤波开始****/
		for(adcData.handle = 2;adcData.handle <= adcData.tail;adcData.handle++){
			adcData.adc[adcData.handle-1] = (adcData.adc[adcData.handle]+adcData.adc[adcData.handle-2])>>1;
		}
		/****滤波结束****/
		//********************回波时间应该在50us~200us之间******************************/
		/*	noiseStart_i 去除盲区开始i值  20us~50us处为噪音  noiseStop_i 噪音接收处i值  	 */		
		for(adcData.handle=0;adcData.handle<adcData.tail;adcData.handle++){					//数值装换，找到最大值及其盲区位置i,及平均值
			adcData.adc[adcData.handle] = adcData.adc[adcData.handle]>>3;							//数值转换，电压值5.12V
			
			if(adcData.handle>10){
				if(adcData.handle<20){																		//盲区最大值及其平均值，包络
					if(adcData.adc[adcData.handle]>Now_amp->Noise_max)										//噪音最大值
						Now_amp->Noise_max=adcData.adc[adcData.handle];
					Now_amp->Noise_avr+=adcData.adc[adcData.handle];											//噪音平均值
					noiseCounter++;
					noiseStop_i=adcData.handle;																						//盲区取样结束处，盲区结束处
				}else{
					if(firstSignal_i==0){																									//寻找包络位置点标志
						if(adcData.adc[adcData.handle]>MAX_SIGNAL_VALUE)	//满刻度且有包络
						{																							//寻找空气状态下第一包络起始位置
							firstSignal_i = adcData.handle;												//第一包络开始点
							secondSignal_i = 3*firstSignal_i;																//第二包络开始点
						}
					}
					if(adcData.adc[adcData.handle] > Now_amp->Amp_Max_Value){		//寻找最大值
						Now_amp->Amp_Max_i = adcData.handle;
						Now_amp->Amp_Max_Value = adcData.adc[adcData.handle];
					}
					
					Now_amp->Amp_avr += adcData.adc[adcData.handle];						//平均计算值
					avrCounter++;
					
					if(adcData.adc[adcData.handle] > MAX_SIGNAL_VALUE)
						Now_amp->largeCounter++;
					else if(adcData.adc[adcData.handle] > MEDIUM_SIGNAL_VALUE)
						Now_amp->mediumCounter++;
					else Now_amp->smallCounter++;
				}
			}
		}
		Now_amp->Amp_avr /= avrCounter;
		Now_amp->Noise_avr /= noiseCounter;
		if((Now_amp->Noise_avr > MAX_SIGNAL_VALUE)||(adcData.adc[noiseStop_i]>MAX_SIGNAL_VALUE))			//噪音过大，蓝灯应该闪烁
			gs_out.measureSt = 0x01;
		
		/*用于信号幅度过大导致的单张与双张无法区分的情况	*/	
		if(firstSignal_i!=0){																									//如果找到第1波，则进行第1、2波信号提取
			for(adcData.handle=firstSignal_i;adcData.handle<firstSignal_i+10;adcData.handle++)
				Now_amp->Amp_avr_1 += adcData.adc[adcData.handle];
			
			if((secondSignal_i+10)<adcData.tail){
				for(adcData.handle=secondSignal_i;adcData.handle<secondSignal_i+10;adcData.handle++)
					Now_amp->Amp_avr_2 += adcData.adc[adcData.handle];
			}
			
			Now_amp->Amp_avr_1 /= 10;																	//信号包络平均值
			Now_amp->Amp_avr_2 /= 10;																	//第二包络平均值
		}
		memset(i_temp,0,sizeof(i_temp));
		memset(ampMaxValue,0,sizeof(ampMaxValue));
		/*寻找上升沿与下降沿，如果数据连续4个上升或有两个连续上升大于0.6V则为上升沿*/
		for(adcData.handle=noiseStop_i,flip_flag=0;adcData.handle<adcData.tail;adcData.handle++)
		{
			if((flip_flag&0x01)==0){															//判断为包络的上升沿的条件
				if((((adcData.adc[adcData.handle]+5)<adcData.adc[adcData.handle+2])
					&&((adcData.adc[adcData.handle+1]+5)<adcData.adc[adcData.handle+3]))
					||((adcData.adc[adcData.handle+2]-adcData.adc[adcData.handle])>30))
				{								
					i_temp[Now_amp->Amp_counter]=adcData.handle;								//存储上升沿开始处的adcData.handle值
					flip_flag |=0x01;													//寻找上升沿下降沿标志位。
				}
			}else{
				if(ampMaxValue[Now_amp->Amp_counter]<adcData.adc[adcData.handle]){		//寻找包络对应的最大值
					ampMax_i[Now_amp->Amp_counter] = adcData.handle;
					ampMaxValue[Now_amp->Amp_counter] = adcData.adc[adcData.handle];
				}
				
				if(((adcData.adc[adcData.handle+1]<adcData.adc[adcData.handle])			//判断为包络下降沿的条件
					&&(adcData.adc[adcData.handle+2]<adcData.adc[adcData.handle+1])
					&&(adcData.adc[adcData.handle+3]<adcData.adc[adcData.handle+2])
					&&((adcData.adc[adcData.handle+4]+10)<adcData.adc[adcData.handle]))
					||((adcData.adc[adcData.handle+3]+60)<adcData.adc[adcData.handle]))
				{
					Now_amp->Amp_counter++;
					if(Now_amp->Amp_counter>=20)
						break;
					flip_flag &=(~0x01);				
				}
			}
		}
		/**********单双张开始判断*************/
		if(Now_amp->Amp_counter>1){
			for(Now_amp->i=0;Now_amp->i<Now_amp->Amp_counter;Now_amp->i++)
				Now_amp->ampAvr += ampMaxValue[Now_amp->i];
			Now_amp->ampAvr /= Now_amp->Amp_counter;
		}
		
		if(firstSignal_i == 0)
			gs_out.measureSt  = 1;
		else if((firstSignal_i>STOP_I)||(firstSignal_i<START_I))
			gs_out.measureSt  = 1;
		
		if(Now_amp->largeCounter > 50){
			gs_out.outSt = NONE_ST;
		}else if(Now_amp->largeCounter > 0){
			if(Now_amp->largeCounter >30){
				if(Now_amp->Amp_avr_2 > MAX_SIGNAL_VALUE)
					gs_out.outSt = NONE_ST;
				else
					gs_out.outSt = ONE_ST;
			} else gs_out.outSt = ONE_ST;
			
		}else{
			if(Now_amp->Amp_counter == 0)
				gs_out.outSt = TWO_ST;
			else{
				if(Now_amp->Amp_Max_Value > (Now_amp->Amp_avr+gs_out.adjValue))
					gs_out.outSt = ONE_ST;
				else
					gs_out.outSt = TWO_ST;
			}
		}
		/**********输出**********/
		switch(gs_out.outSt)									//out judgement
		{
			case NONE_ST:{
				gs_out.countNone++;
				gs_out.countOne=0;
				gs_out.countTwo=0;
				if(gs_out.countNone>=MAX_FILTER){
					gs_out.countNone = MAX_FILTER;
					gs_out.resultSt = gs_out.outSt;
				}
				break;
			}
			case ONE_ST:{
				gs_out.countNone=0;
				gs_out.countOne++;
				gs_out.countTwo=0;
				if(gs_out.countOne>=MAX_FILTER){
					gs_out.countOne = MAX_FILTER;
					if(gs_out.resultSt != gs_out.outSt){
						oneAmpCounter = 0;
						memset(oneAmpDataTemp,0,sizeof(oneAmpDataTemp));
						gs_out.resultSt = gs_out.outSt;
					}
					oneAmpDataTemp[oneAmpCounter++] = Now_amp->Amp_Max_Value;
					oneAmpCounter %= MAX_ONEAMPCOUNTER;
				}
				break;
			}
			case TWO_ST:{
				gs_out.countNone = 0;
				gs_out.countOne=0;
				gs_out.countTwo++;
				if(gs_out.countTwo>=MAX_FILTER){
					gs_out.countTwo = MAX_FILTER;
					gs_out.resultSt = gs_out.outSt;
				}
				break;
			}
		}
			
		if(gs_out.protectOccur == 0){
			switch(gs_out.resultSt)										//IO and LED output 
			{
				case NONE_ST: {
					NonOut();
					break;
				}
				case ONE_ST:{
					OneOut();
					break;
				}
				case TWO_ST:{
					TwoOut();
					break;
				}
				default:
					break;
			}
		}
	}
}	























