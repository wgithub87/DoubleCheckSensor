//*********************************************************
#ifndef  _BSP_KEYSCAN_H
#define  _BSP_KEYSCAN_H
#include <stdint.h>
//*********************************************************

//---------------------------------------------------------
#define KONE_PIN     P01
//*********************************************************
#define KEY_ONE      0x01
//---------------------------------------------------------
#define  DownKeyEx   0x01
#define  UpKeyEx     0x02
#define  ConnKeyEx   0x04
//---------------------------------------------------------

uint8_t KeyScan(void);
uint8_t KeySearch(uint8_t key_number,uint32_t dly);
void MaskerKey(uint8_t key_number);
#endif

