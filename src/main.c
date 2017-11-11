/**
 *
 */
/* Includes *******************************************************************/
#include "main.h"

#include "defines.h"
#include "stm32f4xx.h"
//#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usart.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mjson.h"
#include <time.h>
#include "wifi.h"
#include "json_parser.h"

#define XMAX 239
#define YMAX 319

char buffer[4096];
char serialBuffer[USART_BUFFER_SIZE];


int getPollutionIndex(char * serialBuffer);




unsigned int t[12];
double v[12];
char s[12];

time_t timeStructure1;




/*
struct air_msg_t {
	struct values_list_t values;
	unsigned int start;
	unsigned int end;
	struct vart_list_t var_list1;

} jsonMsg;
*/

/*
static const struct json_attr_t time_json_attrs[] =
		{ { "status", t_string, .addr.string = status, .len = sizeof(status) },
				{ "message", t_string, .addr.string = message, .len =
						sizeof(message) },
				{ "countryCode", t_string, .addr.string = countryCode, .len =
						sizeof(countryCode) }, { "zoneName", t_string,
						.addr.string = zoneName, .len = sizeof(zoneName) }, {
						"abbreviation", t_string, .addr.string = abbreviation,
						.len = sizeof(abbreviation) }, { "gmtOffset", t_string,
						.addr.string = gmtOffset, .len = sizeof(gmtOffset) },
				{ "dst", t_string, .addr.string = dst, .len = sizeof(dst) }, {
						"timestamp", t_uinteger, .addr.uinteger = &timestamp },
				{ NULL }, };
				*/



void displayTable(uint8_t parNum,struct par_list_str_t *pssl){
	struct tm time1;
	char timeStr[9];
	int j=0;
	int h;

	Clear_Screen(0x0000);


	Display_String(0, 319, "Czas:", LCD_WHITE);
	Display_String(0, 150, "Pyl PM2.5 [ug/m3]:", LCD_WHITE);

	for (int i = 12; i < pssl->par_list[parNum - 1].last_sample_index; i++) {
		timeStructure1 = (time_t) atoi(pssl->par_list[parNum - 1].sample_list[i].t_str);
		time1 = *localtime(&timeStructure1);
		h = time1.tm_hour+2;
		if(h==24){
			h = 0;
		}

		if(h==25){
			h = 1;
		}

		itoa(time1.tm_hour+2,timeStr,10);
		strcat(timeStr,":00");

		//Display_String((i+1)*15, 319, pssl->par_list[parNum - 1].sample_list[i].t_str, LCD_WHITE);
		Display_String((j+1)*15, 319,timeStr, LCD_WHITE);
		Display_String((j+1)*15, 150, pssl->par_list[parNum - 1].sample_list[i].v_str, LCD_WHITE);
		j++;
	}
}


void displayChart(uint8_t parNum,struct par_list_str_t *pssl){
	struct tm time1;
	char timeStr[9];
	int i;
	int j=YMAX-35;
	int h;
	uint16_t val;
	uint16_t prev_val;
	uint16_t color;

	Clear_Screen(0xFFFF);
	Set_Font(&Font8x12);

	//horizontal axis
	Draw_Line(XMAX-20,YMAX-40+5,XMAX-20,20, LCD_BLUE);

	//vertical axis
	Draw_Line(XMAX-20,YMAX-40+5, 20,YMAX-40+5, LCD_BLUE);

	//allowed level - 25 ug
	Draw_Line(XMAX-25*3-20,YMAX-40+5,XMAX-25*3-20,20, LCD_YELLOW);

	Draw_Line(XMAX-20*3-20,YMAX-40+5,XMAX-20*3-20,20, LCD_GREY);
	Draw_Line(XMAX-40*3-20,YMAX-40+5,XMAX-40*3-20,20, LCD_GREY);
	Draw_Line(XMAX-60*3-20,YMAX-40+5,XMAX-60*3-20,20, LCD_GREY);
	Draw_Line(XMAX-80*3-20,YMAX-40+5,XMAX-80*3-20,20, LCD_GREY);

	Display_String(XMAX-25*3-20-6, YMAX-5-8+5, "25", LCD_BLACK);
	Display_String(XMAX-25*3-20+6, YMAX-8+5, "max", LCD_BLACK);

	Display_String(XMAX-40*3-20-6, YMAX-5-8+5, "40", LCD_BLACK);
	Display_String(XMAX-60*3-20-6, YMAX-5-8+5, "60", LCD_BLACK);

	Draw_Line(XMAX-20,20,20,20, LCD_GREY);

	Draw_Line(20,YMAX-40+5,20,20, LCD_GREY);




	Display_String(0, 200, "PM2.5 [ug/m3]", LCD_BLACK);

	for (i = 12; i <= pssl->par_list[parNum - 1].last_sample_index; i++) {
		timeStructure1 = (time_t) atoi(pssl->par_list[parNum - 1].sample_list[i].t_str);
		time1 = *localtime(&timeStructure1);
		h = time1.tm_hour+2;
		if(h==24){
			h = 0;
		}

		if(h==25){
			h = 1;
		}
		val = (uint16_t) atof(pssl->par_list[parNum - 1].sample_list[i].v_str);
		prev_val = (uint16_t) atof(pssl->par_list[parNum - 1].sample_list[i-1].v_str);

		itoa(h,timeStr,10);


		Draw_Line(XMAX-prev_val*3-20,j,XMAX-val*3-20,j-20, LCD_RED);


		Display_String(XMAX-18, j-20+4+5, timeStr, LCD_BLACK);

		j=j-20;
	}
	Set_Font(&Font16x24);
	if(val<=20){
		color = LCD_GREEN;
	} else if(val<=20 && val <25){
		color = LCD_ORANGE;
	} else {
		color = LCD_RED;
	}


	Display_String(50, 180, pssl->par_list[parNum - 1].sample_list[i-1].v_str, color);
}


int main(void) {
	//Fatfs object
	FATFS FatFs;
	//File object
	FIL fil;
	//Free and total space
	uint32_t total, free;
	volatile int res;
	volatile int fileSize;
	uint16_t bytesRead;

	bool resWiFi;
	char * jsonBegin;

	int httpRespLength;
	char lenString[5];

	/* Initialization */
	//Initialize system
	SystemInit();
	//Initialize delays
	TM_DELAY_Init();

	/*
	 res= f_mount(&FatFs, "", 1);


	 if (res == FR_OK) {

	 //Try to open file
	 if (f_open(&fil, "kasia.bmp", FA_OPEN_ALWAYS | FA_READ ) == FR_OK) {

	 fileSize = f_size(&fil);
	 f_read(&fil,buffer, 4096, &bytesRead);

	 //Close file, don't forget this!
	 f_close(&fil);
	 }

	 //Unmount drive, don't forget this!
	 f_mount(0, "", 1);
	 }
	 */

	Init_SysTick();
	Init_GPIO();
	Init_FSMC();
	Init_LCD();

	Clear_Screen(0x0000);
	Set_Font(&Font8x12);
	//Display_String(14, 295, "Start", LCD_WHITE);

	/* Demo */
	//Draw_Image(0, 319, 240, 320, kasia);
	//Delay_ms(3000);
	/* Initialize USART1 at 9600 baud, TX: PA9, RX: PA10 */

	//TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);

	// TM_USART_Puts(USART1, "Hello world...\n\r");

	initWiFiModule();

	Delay_ms(5000);

	//TM_USART_Gets(USART2, serialBuffer, 1024);

	//jsonBegin = strchr(serialBuffer,'[');

	while (1) {
		Clear_Screen(0x0000);
		Set_Font(&Font8x12);
		Display_String(75, 310, "Sending HTTP Request ->", LCD_WHITE);
		httpRespLength = getPollutionIndex(serialBuffer);
		Display_String(90, 310, "HTTP Response with length:", LCD_WHITE);
		itoa(httpRespLength, lenString, 10);
		Display_String(90, 100, lenString, LCD_WHITE);


		 jsonBegin = serialBuffer;
		 for(int i=0;i<6;i++){

		 jsonBegin = strstr(jsonBegin+1, "[");
		 }

		//jsonBegin = strstr(jsonBegin, "{");
		 parseJSONMessage(8, &pssl,jsonBegin);

		int status = 0;



		if (jsonBegin) {
			parseJSONMessage(8, &pssl,jsonBegin);
			//displayTable(8,&pssl);
			displayChart(8,&pssl);
			Delay_ms(600000);

			/*
			Display_String(105, 310, jsonBegin, LCD_WHITE);
			Display_String(120, 310, jsonBegin + 38, LCD_WHITE);
			Display_String(135, 310, jsonBegin + 38 * 2, LCD_WHITE);
			Display_String(150, 310, jsonBegin + 38 * 3, LCD_WHITE);
			Display_String(165, 310, jsonBegin + 38 * 4, LCD_WHITE);
			Display_String(180, 310, jsonBegin + 38 * 5, LCD_WHITE);
			Display_String(195, 310, jsonBegin + 38 * 6, LCD_WHITE);
			Display_String(210, 310, jsonBegin + 38 * 7, LCD_WHITE);
			*/
		} else {
			Display_String(105, 310, serialBuffer, LCD_WHITE);
		}

	}

	return 0;
}

int getPollutionIndex(char * serialBuffer) {
	uint8_t c;
	int httpResponseLength;
	char lenStr[5];
	memset(serialBuffer, 0, 4096);
	TM_USART_ClearBuffer(USART2);



	//bielany
	TM_USART_Puts(USART2,
			"GET /bielany/bielany.php?_dc=1508578334779&filename=bielany.dat&s=1508538734&e=1508578334&vars=050CO%3AA1h%2C050SO2%3AA1h%2C050O3%3AA1h%2C050BZN%3AA1h%2C050PM10%3AA1h%2C050PM25%3AA1h%2C050N HTTP/1.1\r\n");
	TM_USART_Puts(USART2, "Accept-Encoding: gzip,deflate\r\n");
	TM_USART_Puts(USART2, "Host: x34.dacsystem.pl\r\n");
	TM_USART_Puts(USART2, "Connection: Keep-Alive\r\n\r\n");

	Delay_ms(5000);
	int i = 0;

	while (!TM_USART_BufferEmpty(USART2)) {
		c = TM_USART_Getc(USART2);
		//TM_USART_Putc(USART1, c);
		serialBuffer[i] = c;
		i++;

	}
	serialBuffer[i] = 0;

	httpResponseLength = strlen(serialBuffer);

	return httpResponseLength;
}





