#ifndef KELOLA_TERAPI_H
#define KELOLA_TERAPI_H

#include "struktur_data.h"

/* --- REVISI UTAMA ---
 * Prototipe fungsi tidak banyak berubah, hanya memastikan
 * semuanya sesuai dengan implementasi di .c yang sudah direvisi.
 */

// Fungsi utama menu
void kelola_terapi_pasien_menu();

// Fungsi pencarian
Pasien* cari_pasien_by_id(const char* patient_id);

// Fungsi utilitas yang relevan
KlasifikasiUsia get_klasifikasi_usia(int usia);
void generate_patient_id(char* patient_id_str);
int hitung_selisih_bulan(const char* tanggal_awal_str, const char* tanggal_akhir_str);
void hitung_tanggal_berikutnya(const char* tanggal_awal_str, int tambah_bulan, char* tanggal_berikut_str, int buffer_size);

// Fungsi inti kelola terapi
void update_hasil_cek_lab(Pasien* p);
void tentukan_regimen_otomatis(Pasien* p, StatusTerapiARV target_lini);

#endif // KELOLA_TERAPI_H