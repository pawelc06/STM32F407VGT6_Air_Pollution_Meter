/*
 * json_parser.h
 *
 *  Created on: 11 lis 2017
 *      Author: Pawe³
 */

#ifndef SRC_JSON_PARSER_H_
#define SRC_JSON_PARSER_H_

#include <stdint.h>

#define AIR_MAX_SAMPLES 25

struct air_sample_str_t {
	char t_str[11];
	char v_str[6];

};

struct par_sample_str_list_t {
	struct air_sample_str_t sample_list[AIR_MAX_SAMPLES];
	uint8_t last_sample_index;
} psl;

struct par_list_str_t {
	struct par_sample_str_list_t par_list[20];
} pssl;


struct air_sample_t {
	unsigned int t;
	double v;
	char s;
};

struct par_sample_list_t {
	struct air_sample_t sample_list[AIR_MAX_SAMPLES];
} sl;

struct par_list_t {
	struct par_sample_list_t par_list[20];
};

struct values_list_t {
	struct par_list_t value_list[1];
};

struct var_t {
	char var_value[12];
};

struct var_list_t {
	struct var_t var1[20];
};


uint8_t parseJSONMessage(uint8_t parNum, struct par_list_str_t *pssl,char * jsonMsg);


#endif /* SRC_JSON_PARSER_H_ */
