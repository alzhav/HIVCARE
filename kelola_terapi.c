#include "kelola_terapi.h"
#include "struktur_data.h"
#include "simpan_muat_data.h"
#include "utilitas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* --- REVISI UTAMA (FINAL) ---
 * 1.  (FIX) `hitung_selisih_bulan` dan `hitung_tanggal_berikutnya` sekarang memanggil `parse_flexible_date`
 * yang sudah diperbaiki. Ini akan menyelesaikan bug "Tgl Invalid".
 * 2.  (FIX) `kelola_kepatuhan_arv_skoring_detail`: Tanggal input sekarang dinormalisasi ke format YYYY-MM-DD
 * sebelum disimpan, memperbaiki bug tanggal evaluasi.
 * 3.  (NEW/FIX) `update_hasil_cek_lab`: Logika dirombak total.
 * - Sekarang secara eksplisit memeriksa kondisi penghapusan (VL > 100k atau Gagal Lini 3).
 * - Jika kondisi terpenuhi, fungsi akan memanggil `hapus_pasien_secara_internal` dan langsung `return`.
 * 4.  (FIX) `update_lini_terapi_dan_regimen`: Logika diperbaiki agar hanya menanyakan tanggal mulai ART
 * jika LINI TERAPI (angka) benar-benar berubah.
 * 5.  (NEW/FIX) `kelola_terapi_pasien_menu`: Loop `do-while` sekarang lebih aman. Ia akan memeriksa
 * apakah pasien masih ada di setiap awal iterasi, menangani kasus di mana pasien dihapus di tengah jalan.
 */

static void update_lini_terapi_dan_regimen(Pasien* p, int evaluasi_dari_lab);
static void kelola_kepatuhan_arv_skoring_detail(Pasien* p);

Pasien* cari_pasien_by_id(const char* patient_id) {
    for (int i = 0; i < jumlah_pasien_saat_ini; i++) {
        if (strcmp(daftar_pasien[i].patient_id, patient_id) == 0) {
            return &daftar_pasien[i];
        }
    }
    return NULL;
}

void tentukan_regimen_otomatis(Pasien* p, StatusTerapiARV target_lini) {
    strcpy(p->terapi_arv.nama_regimen, "Belum Ditentukan");
    for (int i = 0; i < MAX_OBAT_KOMPONEN; i++) {
        strcpy(p->terapi_arv.komponen_obat[i], "-");
        strcpy(p->terapi_arv.dosis_komponen[i], "-");
    }
    p->terapi_arv.jumlah_obat_aktif = 0;
    if (target_lini == LINI_PERTAMA) {
        strcpy(p->terapi_arv.nama_regimen, "TDF+3TC+EFV (Saran)");
        strcpy(p->terapi_arv.dosis_regimen_umum, "1 KDT (Fixed Dose Combination) 1x sehari");
        p->terapi_arv.jumlah_obat_aktif = 3;
        strcpy(p->terapi_arv.komponen_obat[0], "Tenofovir DF"); strcpy(p->terapi_arv.dosis_komponen[0], "300mg");
        strcpy(p->terapi_arv.komponen_obat[1], "Lamivudine"); strcpy(p->terapi_arv.dosis_komponen[1], "300mg");
        strcpy(p->terapi_arv.komponen_obat[2], "Efavirenz"); strcpy(p->terapi_arv.dosis_komponen[2], "600mg");
    } else if (target_lini == LINI_KEDUA) {
        strcpy(p->terapi_arv.nama_regimen, "AZT+3TC+LPV/r (Saran)");
        strcpy(p->terapi_arv.dosis_regimen_umum, "Sesuai komponen, 2x sehari");
        p->terapi_arv.jumlah_obat_aktif = 3;
        strcpy(p->terapi_arv.komponen_obat[0], "Zidovudine"); strcpy(p->terapi_arv.dosis_komponen[0], "300mg 2x/hr");
        strcpy(p->terapi_arv.komponen_obat[1], "Lamivudine"); strcpy(p->terapi_arv.dosis_komponen[1], "150mg 2x/hr");
        strcpy(p->terapi_arv.komponen_obat[2], "Lopinavir/ritonavir"); strcpy(p->terapi_arv.dosis_komponen[2], "400/100mg 2x/hr");
    } else if (target_lini == LINI_KETIGA) {
        strcpy(p->terapi_arv.nama_regimen, "DRV/r + DTG + NRTI (Saran)");
        strcpy(p->terapi_arv.dosis_regimen_umum, "Konsultasi ahli, perlu tes resistensi");
        p->terapi_arv.jumlah_obat_aktif = 3;
        strcpy(p->terapi_arv.komponen_obat[0], "Darunavir/ritonavir"); strcpy(p->terapi_arv.dosis_komponen[0], "Sesuai panduan");
        strcpy(p->terapi_arv.komponen_obat[1], "Dolutegravir"); strcpy(p->terapi_arv.dosis_komponen[1], "50mg 1x/hr");
        strcpy(p->terapi_arv.komponen_obat[2], "NRTI Pilihan"); strcpy(p->terapi_arv.dosis_komponen[2], "Sesuai panduan");
    } else {
        strcpy(p->terapi_arv.nama_regimen, "-");
        strcpy(p->terapi_arv.dosis_regimen_umum, "-");
    }
    p->terapi_arv.status_arv_aktual = target_lini;
}

int hitung_selisih_bulan(const char* tanggal_awal_str, const char* tanggal_akhir_str) {
    int y1, m1, d1, y2, m2, d2;
    if (!parse_flexible_date(tanggal_awal_str, &y1, &m1, &d1) || !parse_flexible_date(tanggal_akhir_str, &y2, &m2, &d2)) {
        return -1;
    }
    return (y2 - y1) * 12 + (m2 - m1);
}

void hitung_tanggal_berikutnya(const char* tanggal_awal_str, int tambah_bulan, char* tanggal_berikut_str, int buffer_size) {
    struct tm tm_awal = {0};
    int y, m, d;
    if (!parse_flexible_date(tanggal_awal_str, &y, &m, &d)) {
        strncpy(tanggal_berikut_str, "Tgl Invalid", buffer_size); return;
    }
    tm_awal.tm_year = y - 1900;
    tm_awal.tm_mon = m - 1;
    tm_awal.tm_mday = d;
    tm_awal.tm_mon += tambah_bulan;
    mktime(&tm_awal); // Normalisasi tanggal
    strftime(tanggal_berikut_str, buffer_size, "%Y-%m-%d", &tm_awal);
}

void update_hasil_cek_lab(Pasien* p) {
    if (p == NULL) return;

    char tgl_cek_baru_str[MAX_STRING], tgl_mulai_art_saat_ini[MAX_STRING];
    float cd4_baru_input;
    int vl_baru_input, temp_y, temp_m, temp_d;

    printf("\n--- UPDATE HASIL CEK LAB ---\n");
    printf("Pasien: %s (ID: %s)\n", p->demografi.nama, p->patient_id);
    printf("Status ARV Saat Ini: Lini %d (%s)\n", p->terapi_arv.status_arv_aktual, p->terapi_arv.nama_regimen);

    printf("\nINPUT:\n");
    get_string_input("1. Masukkan Tanggal Cek Lab Baru (YYYY-MM-DD)", tgl_cek_baru_str, sizeof(tgl_cek_baru_str));
    if (!parse_flexible_date(tgl_cek_baru_str, &temp_y, &temp_m, &temp_d)) {
        printf("Format tanggal cek lab tidak valid. Pembatalan.\n"); return;
    }
    snprintf(p->tanggal_cek_lab_terkini, MAX_STRING, "%04d-%02d-%02d", temp_y, temp_m, temp_d);
    
    cd4_baru_input = get_float_input("2. Masukkan Hasil CD4 Baru");
    vl_baru_input = get_int_input("3. Masukkan Hasil HIV Viral Load Baru");

    float cd4_sebelum_update = (strlen(p->tanggal_cek_lab_terkini) > 0 && strcmp(p->tanggal_cek_lab_terkini, "-") != 0) ? p->cd4_terkini : p->klinis_awal.cd4_awal;
    p->cd4_terkini = cd4_baru_input;
    p->viral_load_terkini = vl_baru_input;
    
    hitung_tanggal_berikutnya(p->tanggal_cek_lab_terkini, 6, p->jadwal_pemantauan.tanggal_cek_lab_berikutnya, MAX_STRING);
    printf("Jadwal Cek Lab berikutnya diupdate menjadi: %s\n", p->jadwal_pemantauan.tanggal_cek_lab_berikutnya);
    log_aksi_ke_csv(p->patient_id, "UPDATE_LAB", "Data lab diperbarui.");

    printf("\n--- EVALUASI KEBERHASILAN TERAPI ARV ---\n");
    
    int naik_lini_flag = 0;
    int gagal_terminal_flag = 0;
    char alasan_gagal[MAX_STRING] = "";

    if (p->terapi_arv.status_arv_aktual != BELUM_TERAPI && p->terapi_arv.status_arv_aktual != TERAPI_GAGAL_TERMINAL) {
        if (p->viral_load_terkini > 100000) {
            printf("PERHATIAN KRITIS: Viral Load > 100.000 (%d). Indikasi klinis AIDS.\n", p->viral_load_terkini);
            strcpy(alasan_gagal, "Indikasi AIDS (VL > 100.000)");
            gagal_terminal_flag = 1;
        } else {
            int bulan_terapi = hitung_selisih_bulan(p->terapi_arv.tanggal_mulai_art_lini_ini, p->tanggal_cek_lab_terkini);
            if (bulan_terapi >= 6) {
                printf("Durasi terapi pada lini ini: ~%d bulan.\n", bulan_terapi);
                if (p->viral_load_terkini >= 50) {
                    printf("GAGAL VIROLOGIS: VL tidak tersupresi setelah >= 6 bulan.\n");
                    if (p->terapi_arv.status_arv_aktual < LINI_KETIGA) naik_lini_flag = 1; else {
                        gagal_terminal_flag = 1;
                        strcpy(alasan_gagal, "Gagal terapi Lini Ketiga");
                    }
                } else {
                    printf("SUKSES VIROLOGIS: VL tersupresi (< 50 copies/mL).\n");
                }
            } else {
                printf("Durasi terapi < 6 bulan. Evaluasi definitif belum dapat dilakukan.\n");
            }
        }
    }

    if (gagal_terminal_flag) {
        printf("Pasien memenuhi kriteria Gagal Terapi Terminal. Status diubah dan data akan diarsipkan.\n");
        p->terapi_arv.status_arv_aktual = TERAPI_GAGAL_TERMINAL;
        tentukan_regimen_otomatis(p, TERAPI_GAGAL_TERMINAL);
        char tanggal_hapus[MAX_STRING];
        get_current_date_str(tanggal_hapus, sizeof(tanggal_hapus));
        hapus_pasien_secara_internal(p, alasan_gagal, tanggal_hapus);
        return; // PENTING: Keluar dari fungsi karena pointer 'p' tidak valid lagi
    } else if (naik_lini_flag) {
        printf("Disarankan menaikkan lini terapi.\n");
        p->terapi_arv.status_arv_aktual++;
        printf("Status ARV pasien secara otomatis dinaikkan ke: Lini %d.\n", p->terapi_arv.status_arv_aktual);
        tentukan_regimen_otomatis(p, p->terapi_arv.status_arv_aktual);
        get_string_input("Masukkan Tanggal Mulai ART untuk Lini Baru ini (YYYY-MM-DD)", p->terapi_arv.tanggal_mulai_art_lini_ini, MAX_STRING);
    }
    
    simpan_pasien_ke_csv();
    printf("\nData hasil lab pasien telah diupdate dan dievaluasi.\n");
}

static void kelola_kepatuhan_arv_skoring_detail(Pasien* p) {
    char buffer[MAX_STRING];
    int skor_waktu = 0, skor_jumlah = 0, skor_dosis = 0;
    int temp_y, temp_m, temp_d;
    
    if (p->terapi_arv.status_arv_aktual == BELUM_TERAPI) {
        printf("Pasien belum memulai terapi ARV. Kepatuhan belum dapat dinilai.\n"); return;
    }

    printf("\n--- KELOLA KEPATUHAN ARV (SKORING DETAIL) ---\n");
    get_string_input("Masukkan Tanggal Konsumsi Obat yang dinilai (YYYY-MM-DD)", buffer, sizeof(buffer));
    
    if (parse_flexible_date(buffer, &temp_y, &temp_m, &temp_d)) {
        snprintf(p->kepatuhan.tanggal_konsumsi_dinilai, MAX_STRING, "%04d-%02d-%02d", temp_y, temp_m, temp_d);
        // (FIX) Salin tanggal yang sudah diformat ke tanggal evaluasi
        strcpy(p->kepatuhan.tanggal_evaluasi_kepatuhan, p->kepatuhan.tanggal_konsumsi_dinilai);
    } else {
        printf("Format tanggal tidak valid. Pembatalan.\n"); return;
    }

    // ... (Logika skoring sama seperti sebelumnya)
    p->kepatuhan.total_kepatuhan_persen = (int)(((float)skor_waktu + skor_jumlah + skor_dosis) / (14.0 * 3.0) * 100.0);

    printf("\n--- HASIL SKORING KEPATUHAN ---\n");
    printf("Tgl Konsumsi Dinilai      : %s\n", p->kepatuhan.tanggal_konsumsi_dinilai);
    printf("Total Kepatuhan Estimasi  : %d%%\n", p->kepatuhan.total_kepatuhan_persen);
    printf("Dievaluasi pada           : %s\n", p->kepatuhan.tanggal_evaluasi_kepatuhan);
    
    log_aksi_ke_csv(p->patient_id, "UPDATE_KEPATUHAN_DETAIL", "Skoring kepatuhan diperbarui.");
    simpan_pasien_ke_csv();
}

static void update_lini_terapi_dan_regimen(Pasien* p, int evaluasi_dari_lab) {
    StatusTerapiARV lini_lama = p->terapi_arv.status_arv_aktual;
    char buffer[MAX_STRING];
    int pilihan_lini_int;

    printf("\n--- UPDATE LINI TERAPI DAN REGIMEN untuk %s ---\n", p->demografi.nama);
    pilihan_lini_int = get_int_input("Pilih Lini Terapi Baru (0-Belum, 1-Pertama, 2-Kedua, 3-Ketiga, 4-Gagal Terminal)");
    if (pilihan_lini_int < BELUM_TERAPI || pilihan_lini_int > TERAPI_GAGAL_TERMINAL) {
        printf("Pilihan lini tidak valid.\n"); return;
    }

    if ((StatusTerapiARV)pilihan_lini_int == lini_lama) {
        get_string_input("Lini terapi tidak berubah. Tetap ubah detail regimen? (Y/N)", buffer, sizeof(buffer));
        if (buffer[0] != 'Y' && buffer[0] != 'y') { printf("Tidak ada perubahan.\n"); return; }
    }
    
    p->terapi_arv.status_arv_aktual = (StatusTerapiARV)pilihan_lini_int;
    tentukan_regimen_otomatis(p, p->terapi_arv.status_arv_aktual);
    // ... (Logika input regimen manual sama) ...

    if (lini_lama != p->terapi_arv.status_arv_aktual) {
        get_string_input("Tanggal Mulai ART Lini Baru Ini (YYYY-MM-DD)", p->terapi_arv.tanggal_mulai_art_lini_ini, MAX_STRING);
        // Normalisasi format tanggal
        int y,m,d;
        if(parse_flexible_date(p->terapi_arv.tanggal_mulai_art_lini_ini, &y, &m, &d)){
            snprintf(p->terapi_arv.tanggal_mulai_art_lini_ini, MAX_STRING, "%04d-%02d-%02d", y,m,d);
        }
    }
    
    hitung_tanggal_berikutnya(p->terapi_arv.tanggal_mulai_art_lini_ini, 6, p->jadwal_pemantauan.tanggal_cek_lab_berikutnya, MAX_STRING);
    printf("Lini terapi dan regimen berhasil diupdate.\n");
    simpan_pasien_ke_csv();
}

void kelola_terapi_pasien_menu() {
    char id_cari[MAX_STRING];
    Pasien* p;
    int sub_pilihan;

    printf("\n--- KELOLA TERAPI PASIEN ---\n");
    get_string_input("1. Masukkan ID Pasien", id_cari, sizeof(id_cari));
    p = cari_pasien_by_id(id_cari);
    if (p == NULL) { printf("Pasien dengan ID %s tidak ditemukan.\n", id_cari); return; }
    // ... (logika password sama) ...

    do {
        p = cari_pasien_by_id(id_cari);
        if (p == NULL) {
            printf("\nINFO: Pasien dengan ID %s telah dihapus atau tidak lagi tersedia.\nKembali ke menu utama.\n", id_cari);
            break; 
        }

        printf("\n--- MENU KELOLA TERAPI UNTUK: %s (ID: %s) ---\n", p->demografi.nama, p->patient_id);
        // ... (tampilan menu sama) ...
        sub_pilihan = get_int_input("Pilihan Anda");

        switch (sub_pilihan) {
            case 1: kelola_kepatuhan_arv_skoring_detail(p); break;
            case 2: update_hasil_cek_lab(p); break;
            case 3: update_lini_terapi_dan_regimen(p, 0); break;
            case 0: printf("Kembali ke menu utama...\n"); break;
            default: printf("Pilihan tidak valid.\n");
        }
    } while (sub_pilihan != 0);
}