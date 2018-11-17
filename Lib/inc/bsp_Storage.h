/* 
 * File:   APP_Storage.h
 * Author: CHG
 *
 * Created on 2017
 */

#ifndef BSP_STORAGE_H
#define	BSP_STORAGE_H
#include "cmsis_os.h"
#pragma anon_unions

#define NONE_ST			0x00
#define	ONE_ST			0x01
#define	TWO_ST			0x02

#define	MAX_FILTER		0x02
#define	MAX_ONEAMPCOUNTER			50



#define DATA_FLASH_BASE			0x1F000
//**********************************************************




typedef struct
{
	uint16_t adc[256];
	uint8_t head;
	uint8_t tail;
	uint8_t handle;
	uint8_t stataus;
}ADC_DATA_T;


typedef struct
{
	uint8_t		countNone;
	uint8_t		countOne;
	uint8_t		countTwo;
	union{
		uint8_t		u8ResultSt;
		struct{
			uint8_t		resultSt:2;
			uint8_t		protectOccur:2;
		};
	};
	union{
		uint8_t		u8MeasureSt;
		struct{
			uint8_t		outSt:2;
			uint8_t		measureSt:2;
		};
	};
	
	uint8_t		overloadCounter;
	uint8_t 	synStataus;
	uint8_t		strength;
	uint8_t		maxValue;
	uint8_t		avrValue;
	uint8_t		adjValue;
}OUT_T;

extern	OUT_T			gs_out;
extern	uint16_t	twinkleCount;
extern	uint16_t	powerDownControl;
extern	osThreadId	mainID,idleID,CalculateThreadID;
extern	ADC_DATA_T	adcData;
extern	uint16_t Non_Paper_Num,One_Paper_Num,Two_Paper_Num,Out_State;
extern	uint16_t adcChannel;
extern	uint8_t adcMaxCounter;
extern	uint8_t twinkleCounter;
extern	uint16_t oneAmpDataTemp[MAX_ONEAMPCOUNTER];
extern	uint8_t oneAmpCounter;


void InitFMC(void);
#endif	/* APP_STORAGE_H */


