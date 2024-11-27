#pragma once
#include <stdlib.h>

typedef enum JSON_VALUE_TYPE
{
	JSON_NONE	= 0,
	JSON_OBJECT	= 1,
	JSON_ARRAY	= 2,
	JSON_NUMBER	= 3,
	JSON_STRING	= 4,
	JSON_TRUE	= 5,
	JSON_FALSE	= 6,
	JSON_NULL	= 7,
} json_value_type_t;

typedef void* json_generic_t;
typedef long double json_number_t;
typedef wchar_t json_char_t;

typedef struct json_object
{
	wchar_t* name;
	json_value_type_t value_type;
	json_generic_t* value;
} json_object_t;

const size_t STR_DEFAULT_ALLOC_SZ = 32;

json_value_type_t json_parse_tfn(wchar_t* json_text, size_t len, size_t tfn_start);
json_number_t json_parse_number(wchar_t* json_text, size_t len, size_t num_start, size_t* new_ptr);
json_char_t* json_parse_string(wchar_t* json_text, size_t len, size_t str_start, size_t* new_ptr);
json_value_type_t json_parse_value(wchar_t* json_text, size_t len, size_t value_start, size_t *new_ptr);
int json_parse_object(wchar_t* json_text, size_t len, size_t object_start, size_t* new_ptr);
void json_parse(wchar_t* json_text, size_t len);
