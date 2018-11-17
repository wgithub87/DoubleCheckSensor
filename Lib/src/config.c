#include "config.h"


uint32_t adcIrqPriority,timer3IrqPriority;
uint32_t ADC_GetConversionRate()
{
	uint32_t u32AdcClkSrcSel;
	uint32_t u32ClkTbl[4] = {__HXT, 0, 0, __HIRC};
	/* Set the PLL clock frequency */
	u32ClkTbl[1] = PllClock;
	/* Set the system core clock frequency */
	u32ClkTbl[2] = SystemCoreClock;
	/* Get the clock source setting */
	u32AdcClkSrcSel = ((CLK->CLKSEL1 & CLK_CLKSEL1_ADC_S_Msk) >> CLK_CLKSEL1_ADC_S_Pos);
	/* Return the ADC conversion rate */
	return ((u32ClkTbl[u32AdcClkSrcSel]) / (((CLK->CLKDIV & CLK_CLKDIV_ADC_N_Msk) >> CLK_CLKDIV_ADC_N_Pos) + 1) / 21);
}

void Init_Board(void)
{
	SYS_UnlockReg();
	CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk|CLK_PWRCON_OSC10K_EN_Msk);      		// HIRC and LIRC Enable
	CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk|CLK_CLKSTATUS_OSC10K_STB_Msk);
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));
	
	CLK_EnableModuleClock(ADC_MODULE);
	CLK_EnableModuleClock(TMR0_MODULE);
	CLK_EnableModuleClock(WDT_MODULE);
	
	/* Select UART module clock source */
	/* ADC clock source is 22.1184MHz, set divider to 7, ADC clock is 22.1184/7 MHz */
//  CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(1));
	CLK->CLKSEL1 |= (0x03<<2);
	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, 0);
	CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDT_S_LIRC, 0);
	SystemCoreClockUpdate();
	/*---------------------------------------------------------------------------------------------------------*/
	/* Init I/O Multi-function                                                                                 */
	/*---------------------------------------------------------------------------------------------------------*/
	SYS->P0_MFP = 0x00000000;
	SYS->P1_MFP = SYS_MFP_P15_AIN5 ;
	SYS->P2_MFP = 0x00000000;
	SYS->P3_MFP = 0x00000000 ;
	SYS->P4_MFP = SYS_MFP_P47_ICE_DAT | SYS_MFP_P46_ICE_CLK;
	/***********************************************************************************************************/
	InitFMC();
	InitGPIO();
	AMPLIFIER_PIN = 1;
	GPIO_DISABLE_DIGITAL_PATH(P1,BIT5);
	
	adcChannel = SIGNAL_CHANNEL;
	ADC_Open(ADC,ADC_ADCR_DIFFEN_SINGLE_END,ADC_ADCR_ADMD_BURST,0x1 << adcChannel);
	ADC_POWER_ON(ADC);
	
	TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 2000);
	TIMER0->TCSR |= TIMER_TCSR_TDR_EN_Msk;
	TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn);
//	InitPowerDownSupport();
#if DEBUG_EN==0		
	WDT_Open(WDT_TIMEOUT_2POW18, WDT_RESET_DELAY_1026CLK, TRUE, FALSE);
	WDT_EnableInt();
	NVIC_EnableIRQ(WDT_IRQn);
#endif
	SYS_LockReg();
}







