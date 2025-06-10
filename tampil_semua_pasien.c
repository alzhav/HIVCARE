#include "tampil_semua_pasien.h"
#include "struktur_data.h"
#include <stdio.h>
#include <string.h> 

void tampilkan_semua_pasien() {
    const char* status_arv_str;
    int i;

    printf("\n==============================================================================================\n");
    printf("                           DAFTAR SEMUA PASIEN (RINGKAS)\n");
    printf("==============================================================================================\n");

        if (jumlah_pasien_saat_ini == 0) {
            printf("Belum ada data pasien.\n");
            return;
        }

    printf("+------------+-------+-----------------+-----------------+--------------+-----------------------+\n");
    printf("| Patient ID | Usia  | Tgl Diagnosis   | Status ARV      | Kepatuhan(%%) | Jadwal Cek Lab Next   |\n");
    printf("+------------+-------+-----------------+-----------------+--------------+-----------------------+\n");

    for (i = 0; i < jumlah_pasien_saat_ini; i++) {
        Pasien p = daftar_pasien[i];
        switch(p.terapi_arv.status_arv_aktual) { 
            case BELUM_TERAPI: status_arv_str = "Belum Terapi"; break;
            case LINI_PERTAMA: status_arv_str = "Lini Pertama"; break;
            case LINI_KEDUA:   status_arv_str = "Lini Kedua";   break;
            case LINI_KETIGA:  status_arv_str = "Lini Ketiga";  break;
            case TERAPI_GAGAL_TERMINAL: status_arv_str = "Gagal Terminal"; break;
            default:           status_arv_str = "Tidak Diketahui"; break;
        }
        printf("| %-10s | %-5d | %-15.15s | %-15.15s | %-12d | %-21.21s |\n",
            p.patient_id,
            p.demografi.usia,
            strlen(p.demografi.tanggal_diagnosis) > 0 ? p.demografi.tanggal_diagnosis : "-", 
            status_arv_str, 
            p.kepatuhan.total_kepatuhan_persen, 
            strlen(p.jadwal_pemantauan.tanggal_cek_lab_berikutnya) > 0 ? p.jadwal_pemantauan.tanggal_cek_lab_berikutnya : "-" ); 
    }
    printf("+------------+-------+-----------------+-----------------+--------------+-----------------------+\n");
}
