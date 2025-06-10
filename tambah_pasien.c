#include "tambah_pasien.h"
#include "struktur_data.h"
#include "simpan_muat_data.h"
#include "kelola_terapi.h"    
#include "utilitas.h" 
        
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void tambah_pasien_baru() {
    Pasien p;
    char buffer[MAX_STRING]; 
    char detail_log[MAX_CATATAN * 2]; 
    int i; 
    StatusTerapiARV target_lini_awal; 

    if (jumlah_pasien_saat_ini >= MAX_PATIENTS) {
        printf("Kapasitas pasien sudah maksimal.\n");
        return;
    }

    // Inisialisasi struct Pasien p dengan nol
    memset(&p, 0, sizeof(Pasien)); 
    generate_patient_id(p.patient_id); 
    printf("Generated Patient ID: %s\n", p.patient_id);

    //Dashboard Pertanyaan Informasi Umum
    printf("==============================================================================================\n");
    printf("                                   INFORMASI UMUM                                             \n");
    printf("==============================================================================================\n");
    get_string_input("1. Nama Lengkap", p.demografi.nama, MAX_STRING);
    p.demografi.usia = get_int_input("2. Usia (tahun)");
    printf("3. Jenis Kelamin (L/P): ");
    get_string_input("", buffer, sizeof(buffer)); 
    if (buffer[0] == 'P' || buffer[0] == 'p') {
        p.demografi.jenis_kelamin = PEREMPUAN;
        p.demografi.hamil = get_int_input("   Apakah pasien hamil? (1=Ya, 0=Tidak)");
    } else {
        p.demografi.jenis_kelamin = LAKI_LAKI;
        p.demografi.hamil = 0; 
    }

    get_string_input("4. Tanggal Diagnosis (YYYY-MM-DD)", p.demografi.tanggal_diagnosis, MAX_STRING);
    get_string_input("5. Dokter Penanggung Jawab", p.demografi.dokter_penanggung_jawab, MAX_STRING);
    get_string_input("6. Perawat", p.demografi.perawat, MAX_STRING);
    get_string_input("7. Konselor", p.demografi.konselor, MAX_STRING);
    get_string_input("8. Catatan Klinis Awal (jika ada, singkat)", p.catatan_klinis_tambahan, MAX_CATATAN);
    get_string_input("9. Password untuk pasien ini", p.demografi.password, MAX_STRING);

    printf("==============================================================================================\n");
    printf("                                   INFO KLINIS AWAL                                           \n");
    printf("==============================================================================================\n");
    p.klinis_awal.berat_badan = get_float_input("1. Berat Badan (kg)");
    p.klinis_awal.tinggi_badan = get_float_input("   Tinggi Badan (cm)");
    p.klinis_awal.cd4_awal = get_float_input("2. CD4 Awal (sel/mm3)");
    p.klinis_awal.hb_awal = get_float_input("3. HB Awal (g/dL)");
    p.klinis_awal.klirens_kreatinin_awal = get_float_input("4. Klirens Kreatinin Awal (ml/menit)");
    printf("5. HBsAg (0=Negatif, 1=Positif): ");
    p.klinis_awal.hbsag = (StatusHBsAg)get_int_input(NULL);
    printf("6. Tes TB (0=Negatif, 1=Positif Laten, 2=Sakit TB Aktif, 3=Tidak Diketahui): ");
    p.klinis_awal.tes_tb = (StatusTesTB)get_int_input(NULL);
    
    p.klinis_awal.pernah_arv_sebelumnya = get_int_input("7. Pernah ARV sebelumnya? (1=Ya, 0=Tidak)");
    if (p.klinis_awal.pernah_arv_sebelumnya == 1) {
        get_string_input("   Detail ARV sebelumnya (Regimen)", p.klinis_awal.detail_arv_sebelumnya, MAX_STRING);
        printf("   Lini Terapi Sebelumnya (0=Belum, 1=Pertama, 2=Kedua, 3=Ketiga): ");
        p.klinis_awal.lini_terapi_sebelumnya = (StatusTerapiARV)get_int_input(NULL);
    } else {
        strcpy(p.klinis_awal.detail_arv_sebelumnya, "-");
        p.klinis_awal.lini_terapi_sebelumnya = BELUM_TERAPI;
    }
    p.klinis_awal.viral_load_awal = get_int_input("8. HIV Viral Load Awal (copies/mL)");
    if (p.demografi.jenis_kelamin == PEREMPUAN) {
        printf("9. Hasil Tes Kehamilan (jika dilakukan) (1=Positif, 0=Negatif, 9=Tidak Dilakukan): ");
        p.klinis_awal.tes_kehamilan_positif = get_int_input(NULL);
    } else {
        p.klinis_awal.tes_kehamilan_positif = 9; 
    }
    p.klinis_awal.gula_darah = get_float_input("10. Gula Darah (mg/dL)");

    // Validasi Viral Load Awal untuk Resiko Progresi AIDS (Langsung Rujukan tanpa Masuk Lini)
    if (p.klinis_awal.viral_load_awal > 100000 && p.klinis_awal.cd4_awal < 200) { 
        printf("PERINGATAN: Viral Load Awal > 100.000 (%d) DAN CD4 Awal < 200 (%.0f).\n", p.klinis_awal.viral_load_awal, p.klinis_awal.cd4_awal);
        printf("Pasien terindikasi sudah dalam kondisi AIDS lanjut. Pertimbangkan rujukan atau evaluasi ahli.\n");

    }

    printf("==============================================================================================\n");
    printf("                          STATUS & TERAPI ARV (Otomatis Ditentukan)                           \n");
    printf("==============================================================================================\n");

    target_lini_awal = LINI_PERTAMA; 
    if (p.klinis_awal.pernah_arv_sebelumnya == 1) {
        target_lini_awal = p.klinis_awal.lini_terapi_sebelumnya;
        if (p.klinis_awal.cd4_awal < 200 || (p.klinis_awal.viral_load_awal > 1000 && p.klinis_awal.viral_load_awal !=0) ) {
            if (target_lini_awal < LINI_KETIGA) {
                target_lini_awal++; 
                printf("Indikasi kegagalan lini sebelumnya (CD4: %.0f, VL: %d). Lini terapi awal disarankan naik ke: Lini %d\n",
                    p.klinis_awal.cd4_awal, p.klinis_awal.viral_load_awal, target_lini_awal + 1); 

            } else if (target_lini_awal == LINI_KETIGA) {
                printf("ERROR: Pasien sudah pada Lini Ketiga dan hasil lab awal menunjukkan potensi kegagalan (CD4: %.0f, VL: %d).\nPasien tidak dapat ditambahkan melalui sistem ini dengan status ini, perlukan evaluasi ahli.\n",
                    p.klinis_awal.cd4_awal, p.klinis_awal.viral_load_awal);
                target_lini_awal = TERAPI_GAGAL_TERMINAL; 
            }

        } else {
            printf("Melanjutkan pada Lini %d berdasarkan riwayat sebelumnya (CD4: %.0f, VL: %d).\n", target_lini_awal + 1, p.klinis_awal.cd4_awal, p.klinis_awal.viral_load_awal);
        }
    } else {
        printf("Pasien belum pernah ARV. Memulai dengan Lini Pertama.\n");
        target_lini_awal = LINI_PERTAMA;
    }
    p.terapi_arv.status_arv_aktual = target_lini_awal;
    tentukan_regimen_otomatis(&p, target_lini_awal); 

    printf("1. Status ARV ditentukan        : Lini %d\n", p.terapi_arv.status_arv_aktual + 1); 
    printf("2. Komponen Obat (disarankan) : ");
    for(i=0; i < p.terapi_arv.jumlah_obat_aktif && i < MAX_OBAT_KOMPONEN; i++) {
        printf("%s (%s)", p.terapi_arv.komponen_obat[i], p.terapi_arv.dosis_komponen[i]);
        if (i < p.terapi_arv.jumlah_obat_aktif -1 && i < MAX_OBAT_KOMPONEN -1) printf(", ");
    }
    printf("\n");
    printf("3. Dosis (Umum Regimen)       : %s\n", p.terapi_arv.dosis_regimen_umum);
    get_string_input("4. Tanggal Mulai ART Lini ini (YYYY-MM-DD)", p.terapi_arv.tanggal_mulai_art_lini_ini, MAX_STRING);

    printf("==============================================================================================\n");
    printf("                        RIWAYAT KESEHATAN LAIN (jika ada, maks %d)\n", MAX_RIWAYAT_KESEHATAN);
    printf("==============================================================================================\n");

    p.jumlah_riwayat_kesehatan = 0;
    for (i = 0; i < MAX_RIWAYAT_KESEHATAN; i++) {
        printf("Tambah riwayat penyakit lain ke-%d? (Y/N): ", i + 1);
        get_string_input("", buffer, sizeof(buffer)); 
        if (buffer[0] != 'Y' && buffer[0] != 'y') {
            break;
        }
        get_string_input("   Nama penyakit/gangguan", p.riwayat_kesehatan[i].nama_penyakit, MAX_STRING);
        get_string_input("   Periode penyakit (mis. 2020-2021)", p.riwayat_kesehatan[i].periode_penyakit, MAX_STRING);
        p.jumlah_riwayat_kesehatan++;
    }

    // Inisialisasi data kepatuhan
    strcpy(p.kepatuhan.tanggal_evaluasi_kepatuhan, "-");
    strcpy(p.kepatuhan.tanggal_konsumsi_dinilai, "-");
    p.kepatuhan.total_kepatuhan_persen = 0; 

    // Inisialisasi data lab terkini dengan data awal
    p.cd4_terkini = p.klinis_awal.cd4_awal;
    p.viral_load_terkini = p.klinis_awal.viral_load_awal;
    if (strlen(p.demografi.tanggal_diagnosis) > 0 && strcmp(p.demografi.tanggal_diagnosis, "-") != 0) {
        strncpy(p.tanggal_cek_lab_terkini, p.demografi.tanggal_diagnosis, MAX_STRING -1);
    } else {
        // Jika tanggal diagnosis tidak ada, bisa set ke tanggal hari ini atau biarkan "-"
        get_current_date_str(p.tanggal_cek_lab_terkini, MAX_STRING);
    }
    p.tanggal_cek_lab_terkini[MAX_STRING -1] = '\0';
    p.hb_terkini = p.klinis_awal.hb_awal;
    p.kreatinin_terkini = p.klinis_awal.klirens_kreatinin_awal; 



    printf("==============================================================================================\n");
    printf("                                 JADWAL PEMANTAUAN\n");
    printf("==============================================================================================\n");

    if (strlen(p.terapi_arv.tanggal_mulai_art_lini_ini) > 0 && strcmp(p.terapi_arv.tanggal_mulai_art_lini_ini, "-") != 0) {
        hitung_tanggal_berikutnya(p.terapi_arv.tanggal_mulai_art_lini_ini, 6, p.jadwal_pemantauan.tanggal_cek_lab_berikutnya, MAX_STRING);
        printf("1. Cek Lab berikutnya (otomatis): %s\n", p.jadwal_pemantauan.tanggal_cek_lab_berikutnya);
    } else {
        strcpy(p.jadwal_pemantauan.tanggal_cek_lab_berikutnya, "Tentukan setelah mulai ART");
        printf("1. Cek Lab berikutnya: %s\n", p.jadwal_pemantauan.tanggal_cek_lab_berikutnya);
    }
    get_string_input("2. Catatan Tambahan (Pemantauan)", p.jadwal_pemantauan.catatan_pemantauan, MAX_CATATAN);

    printf("\n==============================================================================================\n");
    printf("                                    CATATAN PASIEN\n");
    printf("==============================================================================================\n");

    get_string_input("Catatan Tambahan Umum untuk Pasien ini", p.catatan_tambahan_umum, MAX_CATATAN);

    daftar_pasien[jumlah_pasien_saat_ini] = p;
    jumlah_pasien_saat_ini++;

    printf("\nPasien baru dengan ID %s (%s) berhasil ditambahkan.\n", p.patient_id, p.demografi.nama);

    snprintf(detail_log, sizeof(detail_log), "Pasien baru: ID=%s, Nama=%s, Usia=%d, TglDiagnosis=%s, Lini Awal=%d",
            p.patient_id, p.demografi.nama, p.demografi.usia, p.demografi.tanggal_diagnosis, p.terapi_arv.status_arv_aktual + 1); 
    log_aksi_ke_csv(p.patient_id, "TAMBAH_PASIEN", detail_log);
    simpan_pasien_ke_csv(); 
}
