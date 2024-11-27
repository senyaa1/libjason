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

typedef long double json_number_t;
typedef wchar_t json_char_t;

typedef struct json_value json_value_t;

typedef struct json_object
{
	wchar_t* name;
	json_value_t value;
} json_object_t;

typedef struct json_array 
{
	size_t length;
	json_value_t* arr;
} json_array_t;

struct json_value
{
	json_value_type_t type;
	union {
		json_number_t	num;
		json_char_t*	str;
		json_object_t	obj;
		json_array_t	arr;
	} value;
};


const size_t STR_DEFAULT_ALLOC_SZ = 32;

json_number_t json_parse_number(wchar_t* json_text, size_t len, size_t num_start, size_t* new_ptr);
json_char_t* json_parse_string(wchar_t* json_text, size_t len, size_t str_start, size_t* new_ptr);
json_value_t json_parse_value(wchar_t* json_text, size_t len, size_t value_start, size_t *new_ptr);
json_object_t json_parse_object(wchar_t* json_text, size_t len, size_t object_start, size_t* new_ptr);
void json_parse(wchar_t* json_text, size_t len);
