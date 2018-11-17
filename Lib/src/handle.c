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





uint16_t noiseStart_i,noiseStop_i,firstSignal_i,secondSignal_i;										//noiseStart_i:30us��������Ϊ
uint16_t noiseCounter,avrCounter;								//�������ֵ


uint16_t ampMax_i[20],ampMaxValue[20];
uint8_t i_temp[20];							//���ڴ洢�����ؼ����½��ص�iֵ
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
		/****�˲���ʼ****/
		for(adcData.handle = 2;adcData.handle <= adcData.tail;adcData.handle++){
			adcData.adc[adcData.handle-1] = (adcData.adc[adcData.handle]+adcData.adc[adcData.handle-2])>>1;
		}
		/****�˲�����****/
		//********************�ز�ʱ��Ӧ����50us~200us֮��******************************/
		/*	noiseStart_i ȥ��ä����ʼiֵ  20us~50us��Ϊ����  noiseStop_i �������մ�iֵ  	 */		
		for(adcData.handle=0;adcData.handle<adcData.tail;adcData.handle++){					//��ֵװ�����ҵ����ֵ����ä��λ��i,��ƽ��ֵ
			adcData.adc[adcData.handle] = adcData.adc[adcData.handle]>>3;							//��ֵת������ѹֵ5.12V
			
			if(adcData.handle>10){
				if(adcData.handle<20){																		//ä�����ֵ����ƽ��ֵ������
					if(adcData.adc[adcData.handle]>Now_amp->Noise_max)										//�������ֵ
						Now_amp->Noise_max=adcData.adc[adcData.handle];
					Now_amp->Noise_avr+=adcData.adc[adcData.handle];											//����ƽ��ֵ
					noiseCounter++;
					noiseStop_i=adcData.handle;																						//ä��ȡ����������ä��������
				}else{
					if(firstSignal_i==0){																									//Ѱ�Ұ���λ�õ��־
						if(adcData.adc[adcData.handle]>MAX_SIGNAL_VALUE)	//���̶����а���
						{																							//Ѱ�ҿ���״̬�µ�һ������ʼλ��
							firstSignal_i = adcData.handle;												//��һ���翪ʼ��
							secondSignal_i = 3*firstSignal_i;																//�ڶ����翪ʼ��
						}
					}
					if(adcData.adc[adcData.handle] > Now_amp->Amp_Max_Value){		//Ѱ�����ֵ
						Now_amp->Amp_Max_i = adcData.handle;
						Now_amp->Amp_Max_Value = adcData.adc[adcData.handle];
					}
					
					Now_amp->Amp_avr += adcData.adc[adcData.handle];						//ƽ������ֵ
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
		if((Now_amp->Noise_avr > MAX_SIGNAL_VALUE)||(adcData.adc[noiseStop_i]>MAX_SIGNAL_VALUE))			//������������Ӧ����˸
			gs_out.measureSt = 0x01;
		
		/*�����źŷ��ȹ����µĵ�����˫���޷����ֵ����	*/	
		if(firstSignal_i!=0){																									//����ҵ���1��������е�1��2���ź���ȡ
			for(adcData.handle=firstSignal_i;adcData.handle<firstSignal_i+10;adcData.handle++)
				Now_amp->Amp_avr_1 += adcData.adc[adcData.handle];
			
			if((secondSignal_i+10)<adcData.tail){
				for(adcData.handle=secondSignal_i;adcData.handle<secondSignal_i+10;adcData.handle++)
					Now_amp->Amp_avr_2 += adcData.adc[adcData.handle];
			}
			
			Now_amp->Amp_avr_1 /= 10;																	//�źŰ���ƽ��ֵ
			Now_amp->Amp_avr_2 /= 10;																	//�ڶ�����ƽ��ֵ
		}
		memset(i_temp,0,sizeof(i_temp));
		memset(ampMaxValue,0,sizeof(ampMaxValue));
		/*Ѱ�����������½��أ������������4��������������������������0.6V��Ϊ������*/
		for(adcData.handle=noiseStop_i,flip_flag=0;adcData.handle<adcData.tail;adcData.handle++)
		{
			if((flip_flag&0x01)==0){															//�ж�Ϊ����������ص�����
				if((((adcData.adc[adcData.handle]+5)<adcData.adc[adcData.handle+2])
					&&((adcData.adc[adcData.handle+1]+5)<adcData.adc[adcData.handle+3]))
					||((adcData.adc[adcData.handle+2]-adcData.adc[adcData.handle])>30))
				{								
					i_temp[Now_amp->Amp_counter]=adcData.handle;								//�洢�����ؿ�ʼ����adcData.handleֵ
					flip_flag |=0x01;													//Ѱ���������½��ر�־λ��
				}
			}else{
				if(ampMaxValue[Now_amp->Amp_counter]<adcData.adc[adcData.handle]){		//Ѱ�Ұ����Ӧ�����ֵ
					ampMax_i[Now_amp->Amp_counter] = adcData.handle;
					ampMaxValue[Now_amp->Amp_counter] = adcData.adc[adcData.handle];
				}
				
				if(((adcData.adc[adcData.handle+1]<adcData.adc[adcData.handle])			//�ж�Ϊ�����½��ص�����
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
		/**********��˫�ſ�ʼ�ж�*************/
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
		/**********���**********/
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























