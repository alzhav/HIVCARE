#include "struktur_data.h"
#include "simpan_muat_data.h"
#include "tambah_pasien.h"
#include "tampil_semua_pasien.h"
#include "tampil_detail_pasien.h"
#include "kelola_terapi.h"
#include "data_dummy.h"

#include <stdio.h>

Pasien daftar_pasien[MAX_PATIENTS];
int jumlah_pasien_saat_ini = 0;

int main() {
    int pilihan_menu_utama;
    char pilihan_dummy_char;

    muat_semua_data_dari_csv();

    if (jumlah_pasien_saat_ini == 0) {
        printf("Tidak ada data pasien. Apakah Anda ingin membuat data dummy? (y/n): ");
        if (scanf(" %c", &pilihan_dummy_char) == 1) {
            clear_input_buffer();
            if (pilihan_dummy_char == 'y' || pilihan_dummy_char == 'Y') {
                buat_data_dummy();
            }
        } else {
            clear_input_buffer();
            printf("Input tidak valid untuk pilihan data dummy.\n");
        }
    }

    printf(" ==============================================================================================\n");
    printf("               ##     ## #### ##     ##  ######     ###    ########  ########                  \n");
    printf("               ##     ##  ##  ##     ## ##    ##   ## ##   ##     ## ##                        \n");
    printf("               ##     ##  ##  ##     ## ##        ##   ##  ##     ## ##                        \n");
    printf("               #########  ##  ##     ## ##       ##     ## ########  ######                    \n");
    printf("               ##     ##  ##   ##   ##  ##       ######### ##   ##   ##                        \n");
    printf("               ##     ##  ##    ## ##   ##    ## ##     ## ##    ##  ##                        \n");
    printf("               ##     ## ####    ###     ######  ##     ## ##     ## ########                  \n");
    printf(" ==============================================================================================\n");
    printf("  A Sustained HIV Care Monitoring by Luthfan Ali Zhafiri | Crystaly | Fidela Ineziah El Savie  \n");
    printf(" ==============================================================================================\n");


    do {
        printf("\n================================ MENU UTAMA HIVCare+ ================================\n");
        printf("1. Tambah Pasien Baru\n");
        printf("2. Tampilkan Semua Pasien\n");
        printf("3. Tampilkan Detail Pasien\n");
        printf("4. Kelola Terapi Pasien\n");
        printf("5. Pasien Dihapus (Lihat Riwayat/Restore)\n");
        printf("6. Keluar\n");
        printf("======================================================================================\n");
        printf("Pilihan Anda: ");


        if (scanf("%d", &pilihan_menu_utama) != 1) {
            printf("Input tidak valid. Harap masukkan ID yang valid.\n");
            clear_input_buffer();
            pilihan_menu_utama = -1;
            continue;
        }
        clear_input_buffer();

        switch (pilihan_menu_utama) {
            case 1: tambah_pasien_baru(); 
                break;
            case 2: tampilkan_semua_pasien(); 
                break;
            case 3: tampilkan_detail_pasien(); 
                break;
            case 4: kelola_terapi_pasien_menu(); 
                break;
            case 5: menu_penghapusan_pasien_lanjutan(); 
                break;
            case 6:
                printf("Menyimpan data sebelum keluar...\n");
                simpan_semua_data_ke_csv();
                printf("Program selesai. Terima kasih telah menggunakan HIVCare+.\n");
                break;
            default: printf("Pilihan tidak valid. Silakan coba lagi.\n");
        }
    } while (pilihan_menu_utama != 6);

    return 0;
}
