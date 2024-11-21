#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

int read_file(const char* filepath, wchar_t** content);
int write_file(const char* filepath, uint8_t* content, size_t size);
