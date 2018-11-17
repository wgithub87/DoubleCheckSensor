#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "cmsis_os.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "M051Series.h"
#pragma anon_unions

#define DEBUG_EN			0

#if DEBUG_EN
#define DEBUG_MSG   printf
#else
#define DEBUG_MSG(...)
#endif


#define	CALCULATE_MSG			0x0001
#define	PROTECT_MSG				0x0002


#define ADC_OVER_MSG			0x0001
#define ADC_ENABLE_MSG		0x0002





#define	PROTECT_CHANNEL		0x03
#define	SYN_CHANNEL				0x04
#define	SIGNAL_CHANNEL		0x05









void Init_Board(void);

#include "handle.h"
#include "bsp_IntSer.h"
#include "bsp_storage.h"
#include "bsp_stickless.h"
#include "bsp_keyscan.h"
#endif

