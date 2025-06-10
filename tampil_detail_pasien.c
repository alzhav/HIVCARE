// =============== tampil_detail_pasien.c ===============
#include "tampil_detail_pasien.h"
#include "struktur_data.h"
#include "kelola_terapi.h" // Untuk clear_input_buffer dan cari_pasien_by_id
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void tampilkan_detail_pasien() {
    char id_cari[MAX_STRING];
    char pass_input[MAX_STRING];
    Pasien* p;
    const char* status_tes_tb_str;
    const char* status_hbsag_str;
    const char* status_arv_str_detail;
    int i, k_loop;

    printf("\n==============================================================================================\n");
    printf("                              TAMPILKAN DETAIL PASIEN\n");
    printf("==============================================================================================\n");
        printf("1. Pilih ID pasien: ");
        fgets(id_cari, MAX_STRING, stdin);
        id_cari[strcspn(id_cari, "\n")] = 0;

        p = cari_pasien_by_id(id_cari);
        if (p == NULL) {
            printf("Pasien dengan ID %s tidak ditemukan.\n", id_cari);
            return;
        }

        printf("2. Masukkan Password Pasien: ");
        fgets(pass_input, MAX_STRING, stdin);
        pass_input[strcspn(pass_input, "\n")] = 0;

        if (strcmp(p->demografi.password, pass_input) != 0) {
            printf("Password salah. Akses ditolak.\n");
            return;
        }
        printf("BERHASIL\n");


    printf("                         DETAIL PASIEN (ID: %s)\n", p->patient_id);

    printf("\n==============================================================================================\n");
    printf("                                   INFORMASI UMUM \n");
    printf("==============================================================================================\n");
        printf("1. Nama                       : %s\n", strlen(p->demografi.nama) > 0 ? p->demografi.nama : "-");
        printf("2. Usia                       : %d tahun\n", p->demografi.usia);
        printf("3. Jenis Kelamin              : %s\n", p->demografi.jenis_kelamin == LAKI_LAKI ? "Laki-laki" : "Perempuan");
        printf("4. Tanggal Diagnosis          : %s\n", strlen(p->demografi.tanggal_diagnosis) > 0 ? p->demografi.tanggal_diagnosis : "-");
        printf("5. Dokter Penanggung Jawab    : %s\n", strlen(p->demografi.dokter_penanggung_jawab) > 0 ? p->demografi.dokter_penanggung_jawab : "-");
        printf("6. Perawat                    : %s\n", strlen(p->demografi.perawat) > 0 ? p->demografi.perawat : "-");
        printf("7. Konselor                   : %s\n", strlen(p->demografi.konselor) > 0 ? p->demografi.konselor : "-");
        printf("8. Catatan Klinis (Awal)      : %s\n", strlen(p->catatan_klinis_tambahan) > 0 ? p->catatan_klinis_tambahan : "-");
        printf("9. Password (disembunyikan)   : ******\n"); //nanti bakal gua ganti

    printf("\n==============================================================================================\n");
    printf("                                 INFO KLINIS AWAL\n");
    printf("==============================================================================================\n");
        printf("1. Berat/Tinggi Badan         : %.1f kg / %.1f cm\n", p->klinis_awal.berat_badan, p->klinis_awal.tinggi_badan);
        printf("2. CD4 Awal                   : %.2f sel/mm3\n", p->klinis_awal.cd4_awal);
        printf("3. HB Awal                    : %.2f g/dL\n", p->klinis_awal.hb_awal);
        printf("4. Klirens Kreatinin Awal     : %.2f ml/menit\n", p->klinis_awal.klirens_kreatinin_awal);
        switch(p->klinis_awal.hbsag) {
            case HBSAG_NEGATIF: status_hbsag_str = "Negatif"; break;
            case HBSAG_POSITIF: status_hbsag_str = "Positif"; break;
            default: status_hbsag_str = "Tidak Diketahui"; break;
        }
        printf("5. HBsAg                      : %s\n", status_hbsag_str);
        switch(p->klinis_awal.tes_tb) {
            case TES_TB_NEGATIF: status_tes_tb_str = "Negatif"; break;
            case TES_TB_POSITIF_LATEN: status_tes_tb_str = "Positif Laten"; break;
            case TES_TB_SAKIT_AKTIF: status_tes_tb_str = "Sakit TB Aktif"; break;
            default: status_tes_tb_str = "Tidak Diketahui"; break;
        }
        printf("6. Tes TB                     : %s\n", status_tes_tb_str);
        printf("7. Pernah ARV                 : %s (%s, Lini: %d)\n", p->klinis_awal.pernah_arv_sebelumnya ? "Ya" : "Tidak",
                                                p->klinis_awal.pernah_arv_sebelumnya ? p->klinis_awal.detail_arv_sebelumnya : "-",
                                                p->klinis_awal.lini_terapi_sebelumnya);
        printf("8. HIV Viral Load Awal        : %d copies/mL\n", p->klinis_awal.viral_load_awal);
        if (p->demografi.jenis_kelamin == PEREMPUAN) {
                printf("9. Tes Kehamilan (Awal)       : %s\n", p->klinis_awal.tes_kehamilan_positif ? "Positif" : (p->demografi.hamil ? "Hamil (dari info umum)" : "Negatif/Tidak Dilakukan"));
        }
        printf("10. Gula Darah (Awal)         : %.2f mg/dL\n", p->klinis_awal.gula_darah); 

    printf("\n==============================================================================================\n");
    printf("                                 RIWAYAT KESEHATAN\n");
    printf("==============================================================================================\n");
        if (p->jumlah_riwayat_kesehatan > 0) {
            for (i = 0; i < p->jumlah_riwayat_kesehatan; i++) {
                printf("%d. Nama penyakit/gangguan    : %s\n", i + 1, p->riwayat_kesehatan[i].nama_penyakit);
                printf("   Periode penyakit          : %s\n", p->riwayat_kesehatan[i].periode_penyakit);
            }
        } else {
            printf("Tidak ada riwayat kesehatan lain yang tercatat.\n");
        }

    printf("\n==============================================================================================\n");
    printf("                               STATUS & TERAPI ARV\n");
    printf("==============================================================================================\n");
        switch(p->terapi_arv.status_arv_aktual) {
            case BELUM_TERAPI: status_arv_str_detail = "Belum Terapi"; 
                break;
            case LINI_PERTAMA: status_arv_str_detail = "Lini Pertama"; 
                break;
            case LINI_KEDUA:   status_arv_str_detail = "Lini Kedua";   
                break;
            case LINI_KETIGA:  status_arv_str_detail = "Lini Ketiga";  
                break;
            case TERAPI_GAGAL_TERMINAL: status_arv_str_detail = "Gagal Terapi Terminal"; break;
            default:           status_arv_str_detail = "Tidak Diketahui"; break;
        }
        printf("1. Status ARV                 : %s\n", status_arv_str_detail);
        printf("2. Komponen Obat              : ");

        int komponen_ada = 0;
        for(k_loop=0; k_loop < p->terapi_arv.jumlah_obat_aktif && k_loop < MAX_OBAT_KOMPONEN; ++k_loop) {
            if(strlen(p->terapi_arv.komponen_obat[k_loop]) > 0 && strcmp(p->terapi_arv.komponen_obat[k_loop], "-") != 0) {
                if(komponen_ada > 0) printf(", ");
                printf("%s (%s)", p->terapi_arv.komponen_obat[k_loop], strlen(p->terapi_arv.dosis_komponen[k_loop]) > 0 ? p->terapi_arv.dosis_komponen[k_loop] : "-");
                komponen_ada++;
            }
        }

        if(komponen_ada == 0) printf("- (Nama Regimen: %s)", strlen(p->terapi_arv.nama_regimen) > 0 ? p->terapi_arv.nama_regimen : "N/A" );
        printf("\n");
        printf("3. Dosis (Umum Regimen)       : %s\n", strlen(p->terapi_arv.dosis_regimen_umum) > 0 ? p->terapi_arv.dosis_regimen_umum : "-");
        printf("4. Tgl Mulai ART Lini ini     : %s\n", strlen(p->terapi_arv.tanggal_mulai_art_lini_ini) > 0 ? p->terapi_arv.tanggal_mulai_art_lini_ini : "-");
        printf("5. Kepatuhan ARV              : %d%% (Skor Waktu: %d, Jumlah: %d, Dosis: %d, dievaluasi %s)\n",
                p->kepatuhan.total_kepatuhan_persen, p->kepatuhan.skor_ketepatan_waktu,
                p->kepatuhan.skor_jumlah_obat_diminum, p->kepatuhan.skor_dosis_obat,
                strlen(p->kepatuhan.tanggal_evaluasi_kepatuhan) > 0 ? p->kepatuhan.tanggal_evaluasi_kepatuhan : "-");

        printf("6. Resiko Progresi AIDS       : ");
        if (p->viral_load_terkini > 100000) {
            printf("BERPOTENSI AIDS (Viral load > 100.000 copies/mL)\n");
        } else if (p->cd4_terkini < 200 && p->terapi_arv.status_arv_aktual != BELUM_TERAPI) {
            printf("BERPOTENSI AIDS (CD4 < 200 sel/mm3 dan sudah dalam terapi ARV)\n");
        }
        else {
            printf("TIDAK BERPOTENSI AIDS (berdasarkan parameter VL dan CD4 saat ini)\n");
        }

    printf("\n==============================================================================================\n");
    printf("                           INFO KLINIS TERKINI (LAB)\n");
    printf("==============================================================================================\n");

            printf("   CD4 Terkini               : %.2f sel/mm3 (Tgl: %s)\n", p->cd4_terkini, strlen(p->tanggal_cek_lab_terkini) > 0 ? p->tanggal_cek_lab_terkini : "-");
            printf("   Viral Load Terkini        : %d copies/mL (Tgl: %s)\n", p->viral_load_terkini, strlen(p->tanggal_cek_lab_terkini) > 0 ? p->tanggal_cek_lab_terkini : "-");
            printf("   HB Terkini                : %.2f g/dL\n", p->hb_terkini);
            printf("   Kreatinin Terkini         : %.2f mg/dL\n", p->kreatinin_terkini); 

    printf("\n==============================================================================================\n");
    printf("                                JADWAL PEMANTAUAN\n");
    printf("==============================================================================================\n");
            printf("1. Cek Lab berikutnya         : %s\n", strlen(p->jadwal_pemantauan.tanggal_cek_lab_berikutnya) > 0 ? p->jadwal_pemantauan.tanggal_cek_lab_berikutnya : "-");
            printf("2. Catatan Tambahan Pemantauan: %s\n", strlen(p->jadwal_pemantauan.catatan_pemantauan) > 0 ? p->jadwal_pemantauan.catatan_pemantauan : "-");


    printf("\n==============================================================================================\n");
    printf("                            CATATAN TAMBAHAN UMUM\n");
    printf("==============================================================================================\n");

            printf("%s\n", strlen(p->catatan_tambahan_umum) > 0 ? p->catatan_tambahan_umum : "Tidak ada catatan tambahan umum.");
            printf("========================================\n");
}
