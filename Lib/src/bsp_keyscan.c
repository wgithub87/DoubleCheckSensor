/* 
 * File:   BSP_Keyscan.c
 * Author: WCG
 *
 * Created on 20161022
 */
#include "config.h"
//*********************************************************/
//*********************************************************/
typedef struct{
	uint8_t   keyStataus;       					//����״̬
	uint8_t   keyChanged;       					//�ı�İ���
	uint8_t   keyMask;	 							//���εİ���
	uint8_t   dly_keyshort;
	uint32_t   keyDelay;
}Keydef_T;
//*********************************************************/
Keydef_T           key_ram;
//*********************************************************/
uint8_t KeyScan(void)
{
    uint8_t  read_pin=0;
	  
	if(KONE_PIN==0){
		read_pin|=KEY_ONE;
	}
	
	if(read_pin^key_ram.keyStataus){							//�����иı�
		key_ram.keyChanged=(read_pin^key_ram.keyStataus);
	}else{
		if(read_pin==0){							//�����Իָ�
			key_ram.keyStataus = 0;
			key_ram.keyMask = 0;
			key_ram.keyChanged = 0;
		}
	}
	key_ram.keyStataus=read_pin;
	if(key_ram.keyStataus)
		key_ram.keyDelay++;
	else
		key_ram.keyDelay = 0;
	
    return(key_ram.keyStataus||key_ram.keyChanged);
}
//*********************************************************/
uint8_t KeySearch(uint8_t key_number,uint32_t dly)
{
    uint8_t key_data=0;
	
	key_number &= ~key_ram.keyMask;

	if(key_ram.keyChanged&key_number){
		key_ram.keyChanged &= ~key_number;
		if(key_ram.keyStataus&key_number){
			if(dly<key_ram.keyDelay)
				key_data = ConnKeyEx;
			else
				key_data=DownKeyEx;									//����������Ч
		}else{
			key_data=UpKeyEx;										//����̧��
		}
	}else{
		if((key_ram.keyStataus&key_number)&&(dly < key_ram.keyDelay)){
			key_data=ConnKeyEx;
		}
	}
	return(key_data);
}
//*********************************************************/
void MaskerKey(uint8_t key_number)
{
    key_ram.keyMask|=key_number;
}
//*********************************************************/
void InitKeyBoardThread(void)
{
	GPIO_SetMode(P0,BIT1,GPIO_PMD_INPUT);
}
//*********************************************************/



//*********************************************************/










