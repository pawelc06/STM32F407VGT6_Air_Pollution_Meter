/*
 * wifi.h
 *
 *  Created on: 11 lis 2017
 *      Author: Pawe³
 */

#ifndef INC_WIFI_H_
#define INC_WIFI_H_
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "tm_stm32f4_usart.h"
#include "stm32f4xx.h"
#include "LCD_STM32F4.h"

void initWiFiModule();
bool enterCommandMode();


#endif /* INC_WIFI_H_ */
