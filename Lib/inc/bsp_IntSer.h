/* 
 * File:   BSP_IntSer.h
 * Author: WCG
 *
 * Created on 20171009
 */

#ifndef BSP_INTSER_H
#define	BSP_INTSER_H




#define AMP_ON()		P31=1
#define AMP_OFF()		P31=0








#define ADC_CHANNEL			0x03
#define BLUE_LED				P22
#define GREEN_LED				P23
#define RED_LED					P04

#define InvalidOut()		{P05=0;P06=0;P07=0;}
#define NonOut()				{P05=1;P06=0;P07=0;} 
#define OneOut()				{P05=0;P06=1;P07=0;}
#define TwoOut()				{P05=0;P06=0;P07=1;}

#define ALL_LED_OFF()		{P22=1;P23=1;P04=1;}
#define ALL_LED_ON()		{P22=0;P23=0;P04=0;}
#define BLUE_ON()				{P22=0;}
#define GREEN_ON()			{P23=0;}
#define RED_ON()				{P04=0;}
#define BLUE_OFF()			{P22=1;}
#define GREEN_OFF()			{P23=1;}
#define RED_OFF()				{P04=1;}
#define	AMPLIFIER_PIN		P31
#define	CONTROL_PIN			P01

#define PULSE_IO				P26














void InitGPIO(void);
void PulseOut(void);
void StartAdcConvertion(uint8_t channel,uint8_t pulse);
#endif	

