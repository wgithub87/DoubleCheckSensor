/* 
 * File:   storage.c
 * Author: WCG
 *
 * Created on 20171228
 */

#include "config.h"



uint16_t Non_Paper_Num=0,One_Paper_Num=0,Two_Paper_Num=0,Out_State=0;

uint16_t	twinkleCount;
uint16_t	powerDownControl;

osThreadId 		mainID,idleID,CalculateThreadID;

OUT_T		gs_out;
ADC_DATA_T adcData;

uint16_t adcChannel;
uint8_t adcMaxCounter;

uint8_t twinkleCounter;
uint16_t oneAmpDataTemp[MAX_ONEAMPCOUNTER];
uint8_t oneAmpCounter;




void FMC_Init(void)
{
	uint32_t  au32Config[2];
	
	SYS_UnlockReg();
	FMC_Open();
	FMC_ReadConfig(au32Config, 2);
	if (((au32Config[0] & 0x1)) || (au32Config[1] != DATA_FLASH_BASE))
    {
		FMC_ENABLE_CFG_UPDATE();
		FMC_Erase(FMC_CONFIG_BASE);
		au32Config[0] &= ~0x1;
		au32Config[1] = DATA_FLASH_BASE;
		FMC_WriteConfig(au32Config, 2);
		SYS_ResetChip();
	}
	FMC_ReadConfig(au32Config, 2);
}
void InitFMC(void)
{
	uint32_t u32data;
	FMC_Init();
	SYS_UnlockReg();
	u32data = FMC_Read(DATA_FLASH_BASE);
	if(u32data == 0xFFFFFFFF)
	{
		gs_out.adjValue = 0;
		FMC_Write(DATA_FLASH_BASE,gs_out.adjValue);
	}
	else
	{
		if(u32data>200)
			gs_out.adjValue = 200;
		else 
			gs_out.adjValue = u32data;
	}
}





































