#ifndef STICKLESS_H
#define STICKLESS_H



#define POWERDOWN_EN			1









#define OS_CLEAR_WAKEUP_RECORD()		gInterruptFlag=0
#define OS_INTERRUPT_RECORD()			gInterruptFlag|=0x01
#define OS_WAKEUP_RECORD()				gInterruptFlag|=0x80
#define OS_GET_WAKEUP_RECORD()			gInterruptFlag
#define WAKEUP_LED_ON()							P07=1
#define WAKEUP_LED_OFF()						P07=0





void InitPowerDownSupport(void);
void EnterPowerDown(void);

#endif







