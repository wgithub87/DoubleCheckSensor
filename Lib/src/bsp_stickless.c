
#include "config.h"

uint8_t gInterruptFlag;



void InitPowerDownSupport(void)
{
	SYS_UnlockReg();
	CLK_EnableModuleClock(TMR3_MODULE);
	CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3_S_LIRC, 0);
	TIMER_Open(TIMER3, TIMER_ONESHOT_MODE, 10000);
	TIMER3->TCMPR = 10000;
	TIMER3->TCSR |= TIMER_TCSR_TDR_EN_Msk;
	TIMER_EnableWakeup(TIMER3);
	TIMER_EnableInt(TIMER3);
	NVIC_EnableIRQ(TMR3_IRQn);
	TIMER_Start(TIMER3);
}
void EnterPowerDown(void)
{
	uint32_t rt; 
	if(powerDownControl){
		OS_CLEAR_WAKEUP_RECORD();
		
		rt = os_suspend();
		if(rt)
		{
			WAKEUP_LED_OFF(); 
			TIMER3->TCMPR = rt*10;
			TIMER_Start(TIMER3);
			TIMER_EnableInt(TIMER3);
			
			while(TIMER_IS_ACTIVE(TIMER3)==0);
			if(OS_GET_WAKEUP_RECORD() == 0) {
				SYS_UnlockReg();
				CLK_Idle();
			}
		}
		WAKEUP_LED_ON();
		os_resume(rt);
	}
}

void TMR3_IRQHandler(void)
{
	OS_INTERRUPT_RECORD();
	TIMER_ClearIntFlag(TIMER3);
	TIMER_Stop(TIMER3);
}
































