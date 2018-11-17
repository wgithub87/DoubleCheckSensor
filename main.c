/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "config.h"

osThreadDef(CalculateThread, osPriorityNormal, 1, 512);
/*
 * main: initialize and start the system
 */
#define KEY_PERIOD			20
uint16_t keyStick;
uint16_t	saveFlag;
uint8_t keyStatus;
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	Init_Board();
	CalculateThreadID = osThreadCreate(osThread(CalculateThread), NULL);
	mainID = osThreadGetId();
	osKernelStart ();                         // start thread execution 
	
	while(1){
		SYS_UnlockReg();
		WDT_RESET_COUNTER();
		/*
		if(CONTROL_PIN)
			AMPLIFIER_PIN = 1;
		else
			AMPLIFIER_PIN = 0;										//default two amplifiers is action
		*/
		keyStick++;
		if(keyStick > KEY_PERIOD){
			if(KeyScan()){
				keyStatus = KeySearch(KEY_ONE,50);
				if(DownKeyEx&keyStatus){
					gs_out.adjValue += 20;
					if(gs_out.adjValue > 200)
						gs_out.adjValue = 200;
					saveFlag |= 0x01;
				}
				if(ConnKeyEx&keyStatus){
					gs_out.adjValue = 20;
					saveFlag |= 0x01;
				}
				if(UpKeyEx&keyStatus)
					saveFlag |= 0x10;
				keyStick %= KEY_PERIOD;
			}
			if(saveFlag == 0x11){
				if(keyStick > 2000){
					keyStick = 0;
					saveFlag = 0;
					SYS_UnlockReg();
					FMC_Erase(DATA_FLASH_BASE);
					FMC_Write(DATA_FLASH_BASE,gs_out.adjValue);
				}
			}
		}
		
		osSignalSet(CalculateThreadID,ADC_ENABLE_MSG);
		osDelay(5);
		twinkleCounter++;
		if(gs_out.protectOccur == 0){
			switch(gs_out.resultSt)										//IO and LED output 
			{
				case NONE_ST: {
					GREEN_OFF();
					RED_OFF();
					if(gs_out.measureSt == 1){
						if(twinkleCounter > 100){
							twinkleCounter = 0;
							BLUE_LED ^= 1;
						}
					}else{
						BLUE_ON();
					}
					break;
				}
				case ONE_ST:{
					uint8_t i;
					uint32_t sumValue=0;
					BLUE_OFF();
					RED_OFF();
					for(i=0;i<MAX_ONEAMPCOUNTER;i++){
						if(oneAmpDataTemp[i])
							sumValue += (oneAmpDataTemp[i] - gs_out.adjValue);
					}
					if(i){
						sumValue /= i;
						sumValue = abs(500-sumValue)/4;
					}
					if(twinkleCounter>=sumValue){
						twinkleCounter -= sumValue;
						GREEN_LED ^= 1;
					}
					break;
				}
				case TWO_ST:{
					BLUE_OFF();
					GREEN_OFF();
					RED_ON();
					break;
				}
				default:
					break;
			}
		}else ALL_LED_ON();
	}
}


	
	
	

