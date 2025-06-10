#include "utilitas.h"
#include <ctype.h>

/* --- REVISI UTAMA (FINAL) ---
 * 1.  Fungsi `parse_flexible_date` ditulis ulang total agar jauh lebih kuat dan andal.
 * - Ia akan membersihkan string input dari semua karakter non-digit.
 * - Ia hanya akan memproses string digit yang panjangnya tepat 8 (untuk YYYYMMDD).
 * - Ini adalah perbaikan inti untuk semua masalah tanggal.
 * 2.  Fungsi `get_string_input`, `get_int_input`, `get_float_input` disempurnakan dari revisi sebelumnya
 * untuk penanganan input yang lebih konsisten.
 */

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void get_string_input(const char* prompt, char* buffer, int size) {
    if (prompt != NULL && strlen(prompt) > 0) {
        printf("%s: ", prompt);
    }
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    } else {
        buffer[0] = '\0';
        if (!feof(stdin)) {
            clear_input_buffer();
        }
    }
}

int get_int_input(const char* prompt) {
    char buffer_input[MAX_STRING];
    long value;
    char* endptr;
    while (1) {
        get_string_input(prompt, buffer_input, sizeof(buffer_input));
        if (buffer_input[0] == '\0') {
            printf("Input tidak boleh kosong. Masukkan angka integer.\n");
            continue;
        }
        value = strtol(buffer_input, &endptr, 10);
        if (endptr != buffer_input && *endptr == '\0') {
            return (int)value;
        }
        printf("Input tidak valid. Masukkan angka integer saja.\n");
    }
}

float get_float_input(const char* prompt) {
    char buffer_input[MAX_STRING];
    float value;
    char* endptr;
    while (1) {
        get_string_input(prompt, buffer_input, sizeof(buffer_input));
        if (buffer_input[0] == '\0') {
            printf("Input tidak boleh kosong. Masukkan angka desimal.\n");
            continue;
        }
        value = strtof(buffer_input, &endptr);
        if (endptr != buffer_input && *endptr == '\0') {
            return value;
        }
        printf("Input tidak valid. Masukkan angka desimal (gunakan titik '.').\n");
    }
}

int parse_flexible_date(const char* date_str, int* year, int* month, int* day) {
    if (date_str == NULL || year == NULL || month == NULL || day == NULL) return 0;

    char clean_str[MAX_STRING] = {0};
    int i = 0, j = 0;

    // Salin hanya digit dari input string
    for (i = 0; date_str[i] != '\0' && j < sizeof(clean_str) - 1; i++) {
        if (isdigit((unsigned char)date_str[i])) {
            clean_str[j++] = date_str[i];
        }
    }

    // Jika setelah dibersihkan, panjangnya tepat 8, coba parse sebagai YYYYMMDD
    if (strlen(clean_str) == 8) {
        if (sscanf(clean_str, "%4d%2d%2d", year, month, day) == 3) {
            // Validasi dasar bulan dan hari
            if (*month >= 1 && *month <= 12 && *day >= 1 && *day <= 31) {
                return 1; // Sukses
            }
        }
    }
    
    // Jika gagal, coba parse format YYYY-MM-DD sebagai fallback
    if (sscanf(date_str, "%d-%d-%d", year, month, day) == 3) {
        if (*month >= 1 && *month <= 12 && *day >= 1 && *day <= 31) {
            return 1; // Sukses
        }
    }

    return 0; // Gagal parse
}