#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <wchar.h>
#include <string.h>
#include <wctype.h>
#include <math.h>

#include "io.h"
#include "json.h"


static inline bool is_whitespace(wchar_t sym)
{
	return sym == L' ' || sym == L'\t' || sym == L'\r' || sym == L'\n';
}

static inline size_t get_text_start(wchar_t* json_text, size_t len, size_t start)
{
	size_t actual_start = start;
	while(actual_start < len)
	{
		if(!is_whitespace(json_text[actual_start++]))
		{
			actual_start--;
			break;
		}
	}

	return actual_start;
}

json_number_t json_parse_number(wchar_t* json_text, size_t len, size_t num_start, size_t* new_ptr)
{
	size_t actual_start = get_text_start(json_text, len, num_start);

	wchar_t *end = 0;
	long double num = wcstold(json_text + actual_start, &end);

	if (errno == ERANGE || isnan(num) || !isfinite(num))
	{
		fprintf(stderr, "error during number parsing!\n");
		return NAN;
	}

	*new_ptr = end - json_text - 1;
	return num;
}


json_char_t* json_parse_string(wchar_t* json_text, size_t len, size_t str_start, size_t* new_ptr)
{
	bool str_started = false, str_ended = false;
	int begin_index = 0, end_index = 0;

	size_t str_len = 0, str_allocated = STR_DEFAULT_ALLOC_SZ;
	json_char_t* str = (wchar_t*)calloc(str_allocated, sizeof(wchar_t));

	bool prev_was_escaped_backslash = false;

	int i = str_start;
	for(; i < len - 1; i++)
	{
		if(str_ended) break;

		if(str_len + 1 >= str_allocated)
		{
			str_allocated *= 2;
			str = (wchar_t*)realloc(str, str_allocated);
		}

		wchar_t sym = json_text[i];

		if(is_whitespace(sym) && !str_started)
			continue;

		if(!is_whitespace(sym) && !str_started && sym != L'"')
		{
			fprintf(stderr, "Encountered symbols before string start (index: %d)!\n", i);
			return 0;
		}

		bool escaped = i > 1 && json_text[i - 1] == '\\' && !prev_was_escaped_backslash;
		if (!escaped && sym == L'\\')
		{
			prev_was_escaped_backslash = false;
			continue;
		}

		if (escaped && sym == L'\\')
		{
			prev_was_escaped_backslash = true;
			str[str_len++] = '\\';
			continue;
		}

		prev_was_escaped_backslash = false;
		
		if(escaped)
		{
			switch(sym)
			{
				default:
					fprintf(stderr, "invalid escape sequence! (\\%c)\n", sym);
					return 0;
				case L'r':
					str[str_len++] = L'\r';
					break;
				case L'n':
					str[str_len++] = L'\n';
					break;
				case L't':
					str[str_len++] = L'\t';
					break;
				case L'b':
					str[str_len++] = L'\b';
					break;
				case L'f':
					str[str_len++] = L'\f';
					break;
				case '/':
				case '"':
					str[str_len++] = sym;
					break;

				case 'u':			// parse things such as \u002F (/)
					errno = 0;
					wchar_t hex[5] = { 0 };
					wchar_t *end = 0;

					if(len - i - 1 < 4)
					{
			            		fprintf(stderr, "can't parse unicode code point cause string is out of bounds!\n");
						return 0;
					}

					memcpy(hex, json_text + i + 1, sizeof(wchar_t) * 5);

					wchar_t val = wcstol(json_text + i + 1, &end, 16);

				        if (json_text + i + 1 == end || errno == ERANGE)
					{
			            		fprintf(stderr, "unable to parse unicode code point!\n");
						return 0;
					}

					str[str_len++] = val;
					i += 4;
					break;
			}

			continue;
		}

		switch(sym)
		{
			case '"':
				if(!str_started)
				{
					str_started = true;
					begin_index = i;
				}
				else 
				{
					str_ended = true;
					end_index = i;
				}
				break;

			default:
				str[str_len++] = sym;
		}
	}

	*new_ptr = --i;


	return str;
}

json_value_t json_parse_value(wchar_t* json_text, size_t len, size_t value_start, size_t* new_ptr)
{
	json_value_t val = (json_value_t){JSON_NONE, 0};

	size_t actual_start = get_text_start(json_text, len, value_start);

	json_char_t starting_sym = json_text[actual_start];

	if(starting_sym == '"')
	{
		json_char_t* str = json_parse_string(json_text, len, actual_start, new_ptr);
		// fprintf(stderr, "encountered string! %ls\n", str);
		val.type = JSON_STRING;
		val.value.str = str;
		return val;
	}
	
	if(starting_sym == '-' || iswdigit(starting_sym))
	{
		json_number_t num = json_parse_number(json_text, len, actual_start, new_ptr);
		// fprintf(stderr, "encountered number! %Lf\n", num);
		val.type = JSON_NUMBER;
		val.value.num = num;
		return val;
	}

	if(starting_sym == ']')
		return val;	// end of array


	if(starting_sym == '[')
	{
		// fprintf(stderr, "encountered array!\n");
		size_t array_ptr = actual_start + 1;
		json_array_t arr = 0;

		while(1)
		{
			json_value_t elem = json_parse_value(json_text, len, array_ptr + 1, new_ptr);

			if(!elem.type)
			{
				fprintf(stderr, RED "Unable to parse array element! Probably trailing comma\n" RESET);
				return val;
			}
			
			array_ptr = get_text_start(json_text, len, *new_ptr + 1);

			if(json_text[array_ptr] != L',')
				break;
		}

		val.type = JSON_ARRAY;
		val.value.arr = arr;
		return val;
	}

	if(starting_sym == '{')
	{
		// fprintf(stderr, "encountered object!\n");
		json_object_t obj = json_parse_object(json_text, len, actual_start, new_ptr);
		val.type = JSON_OBJECT;
		val.value.obj = obj;
		return val;
	}


	if(len - actual_start < 5) return val;

	// printf("%ls\n", json_text + actual_start);

	#define VAL_CMP(val_str, enum_val)								\
		if(wcsncmp(json_text + actual_start, val_str, wcslen(val_str)) == 0)			\
		{											\
			val.type = enum_val;								\
			return val;									\
		}											\


	VAL_CMP(L"true", JSON_TRUE)
	VAL_CMP(L"false", JSON_FALSE)
	VAL_CMP(L"null", JSON_NULL)
	
	#undef VAL_CMP

	return val;
}

json_object_t json_parse_object(wchar_t* json_text, size_t len, size_t object_start, size_t* new_ptr)
{
	bool object_started = false, object_ended = false, parsed_key = false, waiting_for_new_entry = false;
	json_object_t obj = (json_object_t){L"UNDEFINED", 0};


	for(size_t i = object_start; i < len; i++)
	{
		wchar_t sym = json_text[i];
		if(is_whitespace(sym))
			continue;


		if(object_started && !parsed_key)
		{
			// fprintf(stderr, "was at index \"%d\"\n", i);
			size_t prev = i; 
			obj.name = json_parse_string(json_text, len, i, &i);

			if(prev == i)
			{
				fprintf(stderr, RED "unable to parse key! at %d\n (probably trailing comma)\n" RESET, i);
				return 0;
			}

			parsed_key = true;
			waiting_for_new_entry = false;
			// fprintf(stderr, "parsed object key: \"%ls\"\n", key);
			// fprintf(stderr, "now at\"%d\"\n", i);
			continue;
		}

		switch(sym)
		{
		     case '{':
				object_started = true;
				// fprintf(stderr, "object started!\n");
				// begin parsing string
				break;
		     case '}':
				if(waiting_for_new_entry)
				{
					fprintf(stderr, RED "Trailing comma!\n" RESET);
					return 0;
				}

				// fprintf(stderr, "object ended!\n");
				object_ended = true;
				break;
		     case ':':
				// fprintf(stderr, "beginning to parse \"%ls\" value\n", key);
				// parse value
				obj.value = json_parse_value(json_text, len, i + 1, &i);
				break;
		     case ',':
				waiting_for_new_entry = true;
				parsed_key = false;
				// fprintf(stderr, "waiting for NEW entry!\n");
			     	break;
		}
	}

	return 0;
}


void json_parse(wchar_t* json_text, size_t len)
{
	json_object_t root = (json_object_t){L"[ROOT]", 0};

	size_t end_ptr = 0;
	json_parse_object(json_text, len, 0, &end_ptr);
}

