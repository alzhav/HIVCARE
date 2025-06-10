#ifndef UTILITAS_H
#define UTILITAS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struktur_data.h"

// Prototipe tidak berubah dari revisi sebelumnya, tapi implementasinya di .c akan lebih baik
void get_string_input(const char* prompt, char* buffer, int size);
int get_int_input(const char* prompt);
float get_float_input(const char* prompt);
void clear_input_buffer();
int parse_flexible_date(const char* date_str, int* year, int* month, int* day);

#endif // UTILITAS_H