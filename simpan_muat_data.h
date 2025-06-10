#ifndef SIMPAN_MUAT_DATA_H
#define SIMPAN_MUAT_DATA_H

#include "struktur_data.h" 
#include <stdio.h>         

// Variabel global untuk data riwayat penghapusan
extern RiwayatHapusPasien daftar_riwayat_hapus[MAX_PATIENTS];
extern int jumlah_riwayat_hapus_saat_ini;

// Fungsi utama untuk muat dan simpan data CSV
void muat_semua_data_dari_csv();
void simpan_semua_data_ke_csv();

// Fungsi spesifik untuk CSV
void muat_pasien_dari_csv();
void simpan_pasien_ke_csv();
void muat_riwayat_hapus_dari_csv();
void simpan_riwayat_hapus_ke_csv();

// Fungsi untuk logging histori
void log_aksi_ke_csv(const char* patient_id, const char* aksi, const char* detail_perubahan);

// Fungsi untuk menangani penghapusan pasien dari menu
void hapus_data_pasien_menu();
void hapus_pasien_secara_internal(Pasien* p_target, const char* alasan_penghapusan, const char* tanggal_penghapusan_str);

// Fungsi untuk restore 
void menu_penghapusan_pasien_lanjutan();
void lihat_riwayat_hapus_dari_memori();
void restore_pasien_dari_memori();


#endif // SIMPAN_MUAT_DATA_H
