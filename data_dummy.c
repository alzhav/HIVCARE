// =============== data_dummy.c ===============
#include "data_dummy.h"
#include "struktur_data.h"
#include "simpan_muat_data.h" 
#include "kelola_terapi.h"    
#include "utilitas.h"         
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

// Fungsi untuk membuat data dummy
void buat_data_dummy() {
    // Pastikan ada cukup ruang untuk 5 pasien dummy
    if (jumlah_pasien_saat_ini > MAX_PATIENTS - 5) { 
        printf("Kapasitas pasien hampir penuh (tersisa %d slot, butuh 5). Data dummy tidak ditambahkan.\n", MAX_PATIENTS - jumlah_pasien_saat_ini);
        return;
    }

    printf("Membuat 5 data pasien dummy (semua belum terapi)...\n");

    Pasien p1, p2, p3, p4, p5; // Deklarasi 5 pasien
    char detail_log[MAX_CATATAN * 2];
    char current_date_buffer[MAX_STRING]; 
    
    // Panggil get_current_date_str setelah deklarasi variabel
    // Asumsi get_current_date_str didefinisikan di simpan_muat_data.c dan dideklarasikan di struktur_data.h
    get_current_date_str(current_date_buffer, sizeof(current_date_buffer));


    // --- Pasien Dummy 1 (Dewasa, Laki-laki, Belum Terapi, CD4 > 350) ---
    memset(&p1, 0, sizeof(Pasien));
    generate_patient_id(p1.patient_id); 
    strcpy(p1.demografi.nama, "Agus Setiawan");
    strcpy(p1.demografi.nik_id_medis, "3201001122330001");
    p1.demografi.usia = 32; // USIA_REMAJA_DEWASA
    p1.demografi.jenis_kelamin = LAKI_LAKI;
    strcpy(p1.demografi.tanggal_diagnosis, "2024-01-15");
    strcpy(p1.demografi.dokter_penanggung_jawab, "Dr. Chandra");
    strcpy(p1.demografi.perawat, "Perawat Doni");
    strcpy(p1.demografi.konselor, "Konselor Eka");
    strcpy(p1.demografi.password, "agus123");
    p1.demografi.status_fungsional = FUNGSIONAL_BAIK;
    p1.demografi.hamil = 0;

    p1.klinis_awal.berat_badan = 68.0;
    p1.klinis_awal.tinggi_badan = 172.0;
    p1.klinis_awal.cd4_awal = 400; // CD4 cukup baik
    p1.klinis_awal.viral_load_awal = 55000;
    p1.klinis_awal.hb_awal = 14.5;
    p1.klinis_awal.klirens_kreatinin_awal = 95.0;
    p1.klinis_awal.hbsag = HBSAG_NEGATIF;
    p1.klinis_awal.tes_tb = TES_TB_NEGATIF;
    p1.klinis_awal.pernah_arv_sebelumnya = 0; 
    strcpy(p1.klinis_awal.detail_arv_sebelumnya, "-");
    p1.klinis_awal.lini_terapi_sebelumnya = BELUM_TERAPI;
    p1.klinis_awal.tes_kehamilan_positif = 9; // Tidak dilakukan
    p1.klinis_awal.gula_darah = 100;

    p1.jumlah_riwayat_kesehatan = 0; 

    p1.terapi_arv.status_arv_aktual = BELUM_TERAPI; 
    tentukan_regimen_otomatis(&p1, LINI_PERTAMA); // Menyarankan Lini Pertama jika akan mulai
    strcpy(p1.terapi_arv.tanggal_mulai_art_lini_ini, "-"); 

    p1.kepatuhan.skor_ketepatan_waktu = 0;
    p1.kepatuhan.skor_jumlah_obat_diminum = 0;
    p1.kepatuhan.skor_dosis_obat = 0;
    p1.kepatuhan.total_kepatuhan_persen = 0;
    strcpy(p1.kepatuhan.tanggal_evaluasi_kepatuhan, "-");
    strcpy(p1.kepatuhan.tanggal_konsumsi_dinilai, "-");
    p1.kepatuhan.terlambat_hari_konsumsi = 0;
    p1.kepatuhan.jumlah_obat_tidak_dikonsumsi = 0;

    p1.cd4_terkini = p1.klinis_awal.cd4_awal;
    p1.viral_load_terkini = p1.klinis_awal.viral_load_awal;
    strcpy(p1.tanggal_cek_lab_terkini, p1.demografi.tanggal_diagnosis);
    p1.hb_terkini = p1.klinis_awal.hb_awal;
    p1.kreatinin_terkini = p1.klinis_awal.klirens_kreatinin_awal;

    strcpy(p1.jadwal_pemantauan.tanggal_cek_lab_berikutnya, "Belum ditentukan");
    strcpy(p1.jadwal_pemantauan.catatan_pemantauan, "Pasien baru, belum memulai terapi ARV. Edukasi dan konseling.");
    strcpy(p1.catatan_klinis_tambahan, "Tidak ada keluhan spesifik saat ini.");
    strcpy(p1.catatan_tambahan_umum, "Kooperatif, bersedia untuk memulai terapi.");

    daftar_pasien[jumlah_pasien_saat_ini++] = p1;
    snprintf(detail_log, sizeof(detail_log), "Pasien dummy ditambahkan: ID=%s, Nama=%s, Status: Belum Terapi", p1.patient_id, p1.demografi.nama);
    log_aksi_ke_csv(p1.patient_id, "TAMBAH_DUMMY", detail_log);

    // --- Pasien Dummy 2 (Anak Sedang, Perempuan, Belum Terapi, TB Laten) ---
    memset(&p2, 0, sizeof(Pasien));
    generate_patient_id(p2.patient_id);
    strcpy(p2.demografi.nama, "Bunga Lestari");
    strcpy(p2.demografi.nik_id_medis, "3202002233440002");
    p2.demografi.usia = 8; // USIA_ANAK_SEDANG
    p2.demografi.jenis_kelamin = PEREMPUAN;
    p2.demografi.hamil = 0; 
    strcpy(p2.demografi.tanggal_diagnosis, "2024-02-10");
    strcpy(p2.demografi.dokter_penanggung_jawab, "Dr. Anisa (Sp.A)");
    strcpy(p2.demografi.perawat, "Suster Wulan");
    strcpy(p2.demografi.konselor, "-");
    strcpy(p2.demografi.password, "bunga123");
    p2.demografi.status_fungsional = FUNGSIONAL_BAIK;

    p2.klinis_awal.berat_badan = 25.0;
    p2.klinis_awal.tinggi_badan = 125.0;
    p2.klinis_awal.cd4_awal = 700; // CD4 anak 
    p2.klinis_awal.viral_load_awal = 30000;
    p2.klinis_awal.hb_awal = 12.5;
    p2.klinis_awal.klirens_kreatinin_awal = 60.0; 
    p2.klinis_awal.hbsag = HBSAG_NEGATIF;
    p2.klinis_awal.tes_tb = TES_TB_POSITIF_LATEN; 
    p2.klinis_awal.pernah_arv_sebelumnya = 0; 
    strcpy(p2.klinis_awal.detail_arv_sebelumnya, "-");
    p2.klinis_awal.lini_terapi_sebelumnya = BELUM_TERAPI;
    p2.klinis_awal.tes_kehamilan_positif = 9; // Tidak relevan
    p2.klinis_awal.gula_darah = 85;

    p2.jumlah_riwayat_kesehatan = 1;
    strcpy(p2.riwayat_kesehatan[0].nama_penyakit, "Riwayat kejang demam");
    strcpy(p2.riwayat_kesehatan[0].periode_penyakit, "Usia 2 tahun");

    p2.terapi_arv.status_arv_aktual = BELUM_TERAPI;
    tentukan_regimen_otomatis(&p2, LINI_PERTAMA); 
    strcpy(p2.terapi_arv.tanggal_mulai_art_lini_ini, "-"); 

    p2.kepatuhan.skor_ketepatan_waktu = 0;
    p2.kepatuhan.skor_jumlah_obat_diminum = 0;
    p2.kepatuhan.skor_dosis_obat = 0;
    p2.kepatuhan.total_kepatuhan_persen = 0;
    strcpy(p2.kepatuhan.tanggal_evaluasi_kepatuhan, "-");
    strcpy(p2.kepatuhan.tanggal_konsumsi_dinilai, "-");
    p2.kepatuhan.terlambat_hari_konsumsi = 0;
    p2.kepatuhan.jumlah_obat_tidak_dikonsumsi = 0;

    p2.cd4_terkini = p2.klinis_awal.cd4_awal;
    p2.viral_load_terkini = p2.klinis_awal.viral_load_awal;
    strcpy(p2.tanggal_cek_lab_terkini, p2.demografi.tanggal_diagnosis);
    p2.hb_terkini = p2.klinis_awal.hb_awal;
    p2.kreatinin_terkini = p2.klinis_awal.klirens_kreatinin_awal;

    strcpy(p2.jadwal_pemantauan.tanggal_cek_lab_berikutnya, "Belum ditentukan");
    strcpy(p2.jadwal_pemantauan.catatan_pemantauan, "Pasien anak, belum terapi. Pertimbangkan profilaksis INH untuk TB Laten.");
    strcpy(p2.catatan_klinis_tambahan, "TB Laten dikonfirmasi. Rencana profilaksis INH.");
    strcpy(p2.catatan_tambahan_umum, "Didampingi orang tua saat konsultasi.");

    daftar_pasien[jumlah_pasien_saat_ini++] = p2;
    snprintf(detail_log, sizeof(detail_log), "Pasien dummy ditambahkan: ID=%s, Nama=%s, Status: Belum Terapi", p2.patient_id, p2.demografi.nama);
    log_aksi_ke_csv(p2.patient_id, "TAMBAH_DUMMY", detail_log);

    // --- Pasien Dummy 3 (Anak Kecil, Laki-laki, Belum Terapi, Gejala Ringan) ---
    memset(&p3, 0, sizeof(Pasien));
    generate_patient_id(p3.patient_id);
    strcpy(p3.demografi.nama, "Putra Pratama");
    strcpy(p3.demografi.nik_id_medis, "3203003344550003");
    p3.demografi.usia = 4; // USIA_ANAK_KECIL
    p3.demografi.jenis_kelamin = LAKI_LAKI;
    strcpy(p3.demografi.tanggal_diagnosis, "2024-03-20");
    strcpy(p3.demografi.dokter_penanggung_jawab, "Dr. Anisa (Sp.A)");
    strcpy(p3.demografi.perawat, "Suster Wulan");
    strcpy(p3.demografi.konselor, "-");
    strcpy(p3.demografi.password, "putra123");
    p3.demografi.status_fungsional = FUNGSIONAL_SEDANG; 
    p3.demografi.hamil = 0;

    p3.klinis_awal.berat_badan = 15.0;
    p3.klinis_awal.tinggi_badan = 95.0;
    p3.klinis_awal.cd4_awal = 500; 
    p3.klinis_awal.viral_load_awal = 180000;
    p3.klinis_awal.hb_awal = 11.0;
    p3.klinis_awal.klirens_kreatinin_awal = 50.0; 
    p3.klinis_awal.hbsag = HBSAG_NEGATIF;
    p3.klinis_awal.tes_tb = TES_TB_TIDAK_DIKETAHUI; 
    p3.klinis_awal.pernah_arv_sebelumnya = 0; 
    strcpy(p3.klinis_awal.detail_arv_sebelumnya, "-");
    p3.klinis_awal.lini_terapi_sebelumnya = BELUM_TERAPI;
    p3.klinis_awal.tes_kehamilan_positif = 9;
    p3.klinis_awal.gula_darah = 90;

    p3.jumlah_riwayat_kesehatan = 0;

    p3.terapi_arv.status_arv_aktual = BELUM_TERAPI;
    tentukan_regimen_otomatis(&p3, LINI_PERTAMA); 
    strcpy(p3.terapi_arv.tanggal_mulai_art_lini_ini, "-");

    p3.kepatuhan.skor_ketepatan_waktu = 0;
    p3.kepatuhan.skor_jumlah_obat_diminum = 0;
    p3.kepatuhan.skor_dosis_obat = 0;
    p3.kepatuhan.total_kepatuhan_persen = 0;
    strcpy(p3.kepatuhan.tanggal_evaluasi_kepatuhan, "-");
    strcpy(p3.kepatuhan.tanggal_konsumsi_dinilai, "-");
    p3.kepatuhan.terlambat_hari_konsumsi = 0;
    p3.kepatuhan.jumlah_obat_tidak_dikonsumsi = 0;

    p3.cd4_terkini = p3.klinis_awal.cd4_awal;
    p3.viral_load_terkini = p3.klinis_awal.viral_load_awal;
    strcpy(p3.tanggal_cek_lab_terkini, p3.demografi.tanggal_diagnosis);
    p3.hb_terkini = p3.klinis_awal.hb_awal;
    p3.kreatinin_terkini = p3.klinis_awal.klirens_kreatinin_awal;

    strcpy(p3.jadwal_pemantauan.tanggal_cek_lab_berikutnya, "Belum ditentukan");
    strcpy(p3.jadwal_pemantauan.catatan_pemantauan, "Pasien anak kecil, belum terapi. Perlu skrining TB dan evaluasi gejala.");
    strcpy(p3.catatan_klinis_tambahan, "Sering demam dan diare beberapa minggu terakhir. Limfadenopati generalisata.");
    strcpy(p3.catatan_tambahan_umum, "Didampingi ibu saat konsultasi.");

    daftar_pasien[jumlah_pasien_saat_ini++] = p3;
    snprintf(detail_log, sizeof(detail_log), "Pasien dummy ditambahkan: ID=%s, Nama=%s, Status: Belum Terapi", p3.patient_id, p3.demografi.nama);
    log_aksi_ke_csv(p3.patient_id, "TAMBAH_DUMMY", detail_log);

    // --- Pasien Dummy 4 (Dewasa, Perempuan, Belum Terapi, HBsAg Positif) ---
    memset(&p4, 0, sizeof(Pasien));
    generate_patient_id(p4.patient_id);
    strcpy(p4.demografi.nama, "Rina Marlina");
    strcpy(p4.demografi.nik_id_medis, "3204004455660004");
    p4.demografi.usia = 42; 
    p4.demografi.jenis_kelamin = PEREMPUAN;
    p4.demografi.hamil = 0; 
    strcpy(p4.demografi.tanggal_diagnosis, "2023-12-01");
    strcpy(p4.demografi.dokter_penanggung_jawab, "Dr. Wati");
    strcpy(p4.demografi.perawat, "-");
    strcpy(p4.demografi.konselor, "Konselor Budi");
    strcpy(p4.demografi.password, "rina123");
    p4.demografi.status_fungsional = FUNGSIONAL_BAIK;

    p4.klinis_awal.berat_badan = 65.0;
    p4.klinis_awal.tinggi_badan = 158.0;
    p4.klinis_awal.cd4_awal = 550;
    p4.klinis_awal.viral_load_awal = 15000;
    p4.klinis_awal.hb_awal = 13.0;
    p4.klinis_awal.klirens_kreatinin_awal = 90.0;
    p4.klinis_awal.hbsag = HBSAG_POSITIF; 
    p4.klinis_awal.tes_tb = TES_TB_NEGATIF;
    p4.klinis_awal.pernah_arv_sebelumnya = 0; 
    strcpy(p4.klinis_awal.detail_arv_sebelumnya, "-");
    p4.klinis_awal.lini_terapi_sebelumnya = BELUM_TERAPI;
    p4.klinis_awal.tes_kehamilan_positif = 0; 
    p4.klinis_awal.gula_darah = 105;

    p4.jumlah_riwayat_kesehatan = 1;
    strcpy(p4.riwayat_kesehatan[0].nama_penyakit, "Dislipidemia");
    strcpy(p4.riwayat_kesehatan[0].periode_penyakit, "Sejak 2022");
    
    p4.terapi_arv.status_arv_aktual = BELUM_TERAPI;
    tentukan_regimen_otomatis(&p4, LINI_PERTAMA); 
    strcpy(p4.terapi_arv.tanggal_mulai_art_lini_ini, "-");

    p4.kepatuhan.skor_ketepatan_waktu = 0;
    p4.kepatuhan.skor_jumlah_obat_diminum = 0;
    p4.kepatuhan.skor_dosis_obat = 0;
    p4.kepatuhan.total_kepatuhan_persen = 0;
    strcpy(p4.kepatuhan.tanggal_evaluasi_kepatuhan, "-");
    strcpy(p4.kepatuhan.tanggal_konsumsi_dinilai, "-");
    p4.kepatuhan.terlambat_hari_konsumsi = 0;
    p4.kepatuhan.jumlah_obat_tidak_dikonsumsi = 0;

    p4.cd4_terkini = p4.klinis_awal.cd4_awal;
    p4.viral_load_terkini = p4.klinis_awal.viral_load_awal;
    strcpy(p4.tanggal_cek_lab_terkini, p4.demografi.tanggal_diagnosis);
    p4.hb_terkini = p4.klinis_awal.hb_awal;
    p4.kreatinin_terkini = p4.klinis_awal.klirens_kreatinin_awal;

    strcpy(p4.jadwal_pemantauan.tanggal_cek_lab_berikutnya, "Belum ditentukan");
    strcpy(p4.jadwal_pemantauan.catatan_pemantauan, "HBsAg positif, pertimbangkan regimen ARV yang juga aktif untuk Hepatitis B jika memulai terapi. Pantau fungsi hati.");
    strcpy(p4.catatan_klinis_tambahan, "Dislipidemia terkontrol dengan diet.");
    strcpy(p4.catatan_tambahan_umum, "-");

    daftar_pasien[jumlah_pasien_saat_ini++] = p4;
    snprintf(detail_log, sizeof(detail_log), "Pasien dummy ditambahkan: ID=%s, Nama=%s, Status: Belum Terapi", p4.patient_id, p4.demografi.nama);
    log_aksi_ke_csv(p4.patient_id, "TAMBAH_DUMMY", detail_log);

    // --- Pasien Dummy 5 (Dewasa, Laki-laki, Belum Terapi, CD4 Rendah, VL Tinggi) ---
    memset(&p5, 0, sizeof(Pasien));
    generate_patient_id(p5.patient_id);
    strcpy(p5.demografi.nama, "Joko Susilo");
    strcpy(p5.demografi.nik_id_medis, "3205005566770005");
    p5.demografi.usia = 25; 
    p5.demografi.jenis_kelamin = LAKI_LAKI;
    p5.demografi.hamil = 0;
    strcpy(p5.demografi.tanggal_diagnosis, "2024-04-01");
    strcpy(p5.demografi.dokter_penanggung_jawab, "Dr. Chandra");
    strcpy(p5.demografi.perawat, "Perawat Budi");
    strcpy(p5.demografi.konselor, "-");
    strcpy(p5.demografi.password, "joko123");
    p5.demografi.status_fungsional = FUNGSIONAL_SEDANG;

    p5.klinis_awal.berat_badan = 55.5;
    p5.klinis_awal.tinggi_badan = 165.0;
    p5.klinis_awal.cd4_awal = 150; // CD4 rendah
    p5.klinis_awal.viral_load_awal = 300000; // VL tinggi
    p5.klinis_awal.hb_awal = 12.5;
    p5.klinis_awal.klirens_kreatinin_awal = 88.0;
    p5.klinis_awal.hbsag = HBSAG_NEGATIF;
    p5.klinis_awal.tes_tb = TES_TB_NEGATIF;
    p5.klinis_awal.pernah_arv_sebelumnya = 0; 
    strcpy(p5.klinis_awal.detail_arv_sebelumnya, "-");
    p5.klinis_awal.lini_terapi_sebelumnya = BELUM_TERAPI;
    p5.klinis_awal.tes_kehamilan_positif = 9;
    p5.klinis_awal.gula_darah = 92;
    
    p5.jumlah_riwayat_kesehatan = 0;
    
    p5.terapi_arv.status_arv_aktual = BELUM_TERAPI;
    tentukan_regimen_otomatis(&p5, LINI_PERTAMA); 
    strcpy(p5.terapi_arv.tanggal_mulai_art_lini_ini, "-");

    p5.kepatuhan.skor_ketepatan_waktu = 0;
    p5.kepatuhan.skor_jumlah_obat_diminum = 0;
    p5.kepatuhan.skor_dosis_obat = 0;
    p5.kepatuhan.total_kepatuhan_persen = 0;
    strcpy(p5.kepatuhan.tanggal_evaluasi_kepatuhan, "-");
    strcpy(p5.kepatuhan.tanggal_konsumsi_dinilai, "-");
    p5.kepatuhan.terlambat_hari_konsumsi = 0;
    p5.kepatuhan.jumlah_obat_tidak_dikonsumsi = 0;

    p5.cd4_terkini = p5.klinis_awal.cd4_awal;
    p5.viral_load_terkini = p5.klinis_awal.viral_load_awal;
    strcpy(p5.tanggal_cek_lab_terkini, p5.demografi.tanggal_diagnosis);
    p5.hb_terkini = p5.klinis_awal.hb_awal;
    p5.kreatinin_terkini = p5.klinis_awal.klirens_kreatinin_awal;

    strcpy(p5.jadwal_pemantauan.tanggal_cek_lab_berikutnya, "Belum ditentukan");
    strcpy(p5.jadwal_pemantauan.catatan_pemantauan, "CD4 rendah, VL tinggi. Segera mulai ARV Lini Pertama. Edukasi pentingnya kepatuhan.");
    strcpy(p5.catatan_klinis_tambahan, "Pasien tampak lemah, ada penurunan BB.");
    strcpy(p5.catatan_tambahan_umum, "Membutuhkan dukungan nutrisi.");

    daftar_pasien[jumlah_pasien_saat_ini++] = p5;
    snprintf(detail_log, sizeof(detail_log), "Pasien dummy ditambahkan: ID=%s, Nama=%s, Status: Belum Terapi", p5.patient_id, p5.demografi.nama);
    log_aksi_ke_csv(p5.patient_id, "TAMBAH_DUMMY", detail_log);

    printf("%d data pasien dummy berhasil dibuat.\n", 5); 
    simpan_pasien_ke_csv(); 
}