#pragma once

#include "json.h"

#define RED   "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW   "\x1B[33m"
#define BLUE   "\x1B[34m"
#define MAGENTA   "\x1B[35m"
#define CYAN   "\x1B[36m"
#define WHITE   "\x1B[37m"
#define UNDERLINE "\e[4m"
#define RESET "\x1B[0m"
#define BOLD "\e[1m"

void json_print_val(json_value_t *val);
void json_print_arr(json_array_t *arr);
void json_print_obj(json_object_t *obj);
void json_print_str(json_char_t* str);
