/*
 * wifi.c
 *
 *  Created on: 11 lis 2017
 *      Author: Pawe³
 */

#include "wifi.h"
extern char serialBuffer[USART_BUFFER_SIZE];

void initWiFiModule() {
	bool resWiFi;

	TM_GPIO_SetPinHigh(GPIOA, GPIO_Pin_4);
	Delay_ms(500);
	TM_GPIO_SetPinLow(GPIOA, GPIO_Pin_4);
	Delay_ms(300);
	TM_GPIO_SetPinHigh(GPIOA, GPIO_Pin_4);
	Display_String(15, 310, "Waiting for READY...", LCD_WHITE);
	while (TM_GPIO_GetInputPinValue(GPIOC, GPIO_Pin_6))
		;
	Clear_Screen(0x0000);
	Display_String(30, 310, "READY! Waiting for WiFi link...", LCD_WHITE);

	while (!TM_GPIO_GetInputPinValue(GPIOC, GPIO_Pin_7))
		;

	Delay_ms(100);
	Display_String(45, 310, "WiFi connected. Entering command mode", LCD_WHITE);

	resWiFi = enterCommandMode();

	if (resWiFi) {
		Display_String(60, 310, "Success", LCD_WHITE);
	} else {
		Display_String(60, 310, "Failure", LCD_WHITE);
	}

	TM_USART_ClearBuffer(USART2);

	//TM_USART_Puts(USART2,"AT+NETP=TCP,Client,80,192.168.0.120\r\n");

	//aqi.cn
	//TM_USART_Puts(USART2, "AT+NETP=TCP,Client,80,139.162.71.178\r\n");

	//sjp.pl
	//TM_USART_Puts(USART2, "AT+NETP=TCP,Client,80,195.187.34.140\r\n");

	TM_USART_Puts(USART2, "AT+NETP=TCP,Client,80,85.25.104.143\r\n");

	//TM_USART_Puts(USART2,"AT+NETP=TCP,Client,80,192.168.0.31\r\n");
	Delay_ms(50);

	TM_USART_Gets(USART2, serialBuffer, 100);

	Display_String(75, 310, "Response:", LCD_WHITE);
	Display_String(90, 310, serialBuffer, LCD_WHITE);

	Delay_ms(50);
	Display_String(105, 310, "Sending reset", LCD_WHITE);
	TM_USART_ClearBuffer(USART2);
	TM_USART_Puts(USART2, "AT+Z\r\n");
	TM_USART_ClearBuffer(USART2);
	Delay_ms(50);

	while (TM_USART_BufferEmpty(USART2))
		;

	TM_USART_Gets(USART2, serialBuffer, 100);
	Display_String(120, 310, serialBuffer, LCD_WHITE);

	Display_String(15, 310, "Waiting for READY...", LCD_WHITE);
	while (TM_GPIO_GetInputPinValue(GPIOC, GPIO_Pin_6))
		;
	Clear_Screen(0x0000);
	Display_String(30, 310, "READY! Waiting for WiFi link...", LCD_WHITE);

	while (TM_GPIO_GetInputPinValue(GPIOC, GPIO_Pin_7))
		;

	Display_String(45, 310, "WiFi connected. Ready to send HTTP.", LCD_WHITE);

}

bool enterCommandMode() {
	volatile uint8_t serialRes;
	TM_USART_Putc(USART2, '+');
	//Delay_ms(50);
	TM_USART_Putc(USART2, '+');
	//Delay_ms(50);
	TM_USART_Putc(USART2, '+');
	Delay_ms(50);

	if (!TM_USART_FindCharacter(USART2, 'a')) {
		TM_USART_Puts(USART1, "Error! No Ack from wifi module!");
		return false;
	}

	//TM_USART_ClearBuffer(USART2);
	//Delay_ms(50);

	TM_USART_Putc(USART2, 'a');
	//while(TM_USART_BufferEmpty(USART2) == 0);
	Delay_ms(50);

	serialRes = TM_USART_FindCharacter(USART2, '+');

	//TM_USART_Gets(USART2, serialBuffer,3);
	if (!TM_USART_FindCharacter(USART2, '+')) {
		TM_USART_Puts(USART1, "Error! No OK received from wifi module!");
		return false;
	}
	return true;
}
