#ifndef STRUKTUR_DATA_H
#define STRUKTUR_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STRING 100
#define MAX_CATATAN (MAX_STRING * 3)
#define MAX_PATIENTS 100
#define MAX_OBAT_KOMPONEN 5
#define MAX_RIWAYAT_KESEHATAN 5

// Nama file CSV
#define PATIENT_CSV_FILE "patients.csv"
#define DELETED_PATIENTS_CSV_FILE "deleted_patients.csv" 
#define HISTORY_UPDATES_CSV_FILE "history_updates.csv"

// Enum
typedef enum { 
    FUNGSIONAL_BAIK, 
    FUNGSIONAL_SEDANG, 
    FUNGSIONAL_BURUK 
} StatusFungsional;

typedef enum { 
    LAKI_LAKI, 
    PEREMPUAN 
} JenisKelamin;

typedef enum { 
    BELUM_TERAPI, 
    LINI_PERTAMA, 
    LINI_KEDUA, 
    LINI_KETIGA, 
    TERAPI_GAGAL_TERMINAL 
} StatusTerapiARV;

typedef enum { 
    USIA_ANAK_KECIL, 
    USIA_ANAK_SEDANG, 
    USIA_REMAJA_DEWASA 
} KlasifikasiUsia;

typedef enum { 
TES_TB_NEGATIF, 
TES_TB_POSITIF_LATEN, 
TES_TB_SAKIT_AKTIF, 
TES_TB_TIDAK_DIKETAHUI 
} StatusTesTB;

typedef enum { 
    HBSAG_NEGATIF, 
    HBSAG_POSITIF } 
StatusHBsAg;

//STRUCTS
typedef struct {
    char nama_penyakit[MAX_STRING];
    char periode_penyakit[MAX_STRING];
} RiwayatKesehatanTambahan;

typedef struct {
    char nama[MAX_STRING];
    char nik_id_medis[MAX_STRING];
    int usia;
    JenisKelamin jenis_kelamin;
    char tanggal_diagnosis[MAX_STRING];
    char dokter_penanggung_jawab[MAX_STRING];
    char perawat[MAX_STRING];
    char konselor[MAX_STRING];
    char password[MAX_STRING];
    StatusFungsional status_fungsional;
    int hamil; 
} DataDemografi;

typedef struct {
    float berat_badan;
    float tinggi_badan;
    float cd4_awal;
    int viral_load_awal;
    float hb_awal;
    float klirens_kreatinin_awal;
    StatusHBsAg hbsag;
    StatusTesTB tes_tb;
    int pernah_arv_sebelumnya; 
    char detail_arv_sebelumnya[MAX_STRING];
    StatusTerapiARV lini_terapi_sebelumnya;
    int tes_kehamilan_positif; 
    int gula_darah;
} InfoKlinisAwal;

typedef struct {
    StatusTerapiARV status_arv_aktual;
    char nama_regimen[MAX_STRING];
    char komponen_obat[MAX_OBAT_KOMPONEN][MAX_STRING];
    char dosis_komponen[MAX_OBAT_KOMPONEN][MAX_STRING];
    char dosis_regimen_umum[MAX_STRING];
    char tanggal_mulai_art_lini_ini[MAX_STRING];
    int jumlah_obat_aktif;
} RegimenARTTerkini;

typedef struct {
    int skor_ketepatan_waktu;
    int skor_jumlah_obat_diminum;
    int skor_dosis_obat;
    int total_kepatuhan_persen;
    char tanggal_evaluasi_kepatuhan[MAX_STRING];
    char tanggal_konsumsi_dinilai[MAX_STRING];
    int terlambat_hari_konsumsi;
    int jumlah_obat_tidak_dikonsumsi; 
} KepatuhanARV;

typedef struct {
    char tanggal_cek_lab_berikutnya[MAX_STRING];
    char catatan_pemantauan[MAX_CATATAN];
} JadwalPemantauan;

typedef struct {
    char patient_id[MAX_STRING];
    DataDemografi demografi;
    InfoKlinisAwal klinis_awal;
    RiwayatKesehatanTambahan riwayat_kesehatan[MAX_RIWAYAT_KESEHATAN];
    int jumlah_riwayat_kesehatan;
    RegimenARTTerkini terapi_arv;
    KepatuhanARV kepatuhan;

    float cd4_terkini;
    int viral_load_terkini;
    char tanggal_cek_lab_terkini[MAX_STRING];
    float hb_terkini;
    float kreatinin_terkini;

    JadwalPemantauan jadwal_pemantauan;
    char catatan_klinis_tambahan[MAX_CATATAN];
    char catatan_tambahan_umum[MAX_CATATAN];

} Pasien;

// Struktur untuk riwayat pasien yang dihapus 
typedef struct {
    Pasien pasien_yang_dihapus;
    char tanggal_hapus[MAX_STRING];
    char alasan_hapus[MAX_STRING * 2];
} RiwayatHapusPasien;

typedef struct {
    char timestamp[MAX_STRING];     // YYYY-MM-DD HH:MM:SS
    char patient_id[MAX_STRING];
    char aksi[MAX_STRING];          // Notes: TAMBAH, UPDATE_LAB, HAPUS, 
    char detail_perubahan[MAX_CATATAN];
} HistoriUpdateData;

// Variabel global yang akan didefinisikan di file lain
extern Pasien daftar_pasien[MAX_PATIENTS];
extern int jumlah_pasien_saat_ini;

// Fungsi utilitas input -> bakal ke utilitas.c
void clear_input_buffer(); 


// Fungsi utilitas pasien & tanggal ->  di kelola_terapi.c dan utilitas.c
void generate_patient_id(char* patient_id_str);
Pasien* cari_pasien_by_id(const char* patient_id);
KlasifikasiUsia get_klasifikasi_usia(int usia);

// Fungsi utilitas waktu 
void get_current_datetime_str(char* buffer, int buffer_size);
void get_current_date_str(char* buffer, int buffer_size);

int hitung_selisih_bulan(const char* tanggal_awal_str, const char* tanggal_akhir_str);
void hitung_tanggal_berikutnya(const char* tanggal_awal_str, int tambah_bulan, char* tanggal_berikut_str, int buffer_size); 

// Fungsi regiment otomatis ->  didefinisikan di kelola_terapi.c
void tentukan_regimen_otomatis(Pasien* p, StatusTerapiARV target_lini);

#endif 
