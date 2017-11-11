/*
 * json_parser.c
 *
 *  Created on: 11 lis 2017
 *      Author: Pawe³
 */

#ifndef SRC_JSON_PARSER_C_
#define SRC_JSON_PARSER_C_

#include "json_parser.h"


uint8_t parseJSONMessage(uint8_t parNum, struct par_list_str_t *pssl,char * jsonMsg) {

	char *colonPtr;
	char *currStart;
	char val_str[11];
	int i;



	currStart = jsonMsg;
	pssl->par_list[parNum - 1].last_sample_index = AIR_MAX_SAMPLES-1;

	for (i = 0; i < AIR_MAX_SAMPLES; i++) {
		//we look for t
		colonPtr = strchr(currStart, ':');
		pssl->par_list[parNum - 1].sample_list[i].t_str[10] = 0;
		if (colonPtr) {
			currStart = colonPtr + 1;
			strncpy(pssl->par_list[parNum - 1].sample_list[i].t_str, currStart, 10);
		} else {
			return -1;
		}


		//we look for v
		pssl->par_list[parNum - 1].sample_list[i].v_str[4] = 0;
		colonPtr = strchr(currStart, ':');

		if (colonPtr) {
			currStart = colonPtr + 1;
			strncpy(val_str, currStart, 4);
			if(atoi(val_str) > 1000){ //only 24 samples
				pssl->par_list[parNum - 1].last_sample_index = i-1;
				break;
			} else {
				strcpy(pssl->par_list[parNum - 1].sample_list[i].v_str,val_str);
			}
		} else {
			return -1;
		}

		//look for s and skip
		colonPtr = strchr(currStart, ':');
		if (colonPtr) {
			currStart = colonPtr + 1;
		} else {
			return -1;
		}

	}



	return 0;
}

#endif /* SRC_JSON_PARSER_C_ */
