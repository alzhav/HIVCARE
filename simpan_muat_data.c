
#include "simpan_muat_data.h"
#include "struktur_data.h"    
#include "kelola_terapi.h"    
#include "utilitas.h"         
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Variabel global untuk riwayat pasien yang dihapus 
RiwayatHapusPasien daftar_riwayat_hapus[MAX_PATIENTS];
int jumlah_riwayat_hapus_saat_ini = 0;

// Fungsi waktu
void get_current_datetime_str(char* buffer, int buffer_size) {
    time_t t = time(NULL);
    struct tm tm_info = *localtime(&t);

    if (localtime(&t) == NULL) { 
        if (buffer_size > 0) buffer[0] = '\0';
        return;
    }
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", &tm_info);
}

void get_current_date_str(char* buffer, int buffer_size) {
    time_t t = time(NULL);
    struct tm tm_info = *localtime(&t);

    if (localtime(&t) == NULL) { 
        if (buffer_size > 0) buffer[0] = '\0';
        return;
    }
    strftime(buffer, buffer_size, "%Y-%m-%d", &tm_info);
}

// Fungsi pembantu CSV (sesuai kode yang diberikan pengguna)
int baca_csv_line(FILE* file, char fields[200][MAX_STRING], int max_fields, int* num_fields_read) {
    char line[MAX_STRING * 20];
    char* token;
    int i;

    if (fgets(line, sizeof(line), file) == NULL) {
        return 0;
    }
    line[strcspn(line, "\n\r")] = 0;

    *num_fields_read = 0;
    for (i = 0; i < max_fields; i++) {
        fields[i][0] = '\0';
    }

    token = strtok(line, ",");
    while (token != NULL && *num_fields_read < max_fields) {
        if (strlen(token) >= 2 && token[0] == '"' && token[strlen(token)-1] == '"') {
            // Hapus kutip di awal dan akhir, lalu salin
            strncpy(fields[*num_fields_read], token + 1, strlen(token) - 2);
            fields[*num_fields_read][strlen(token) - 2] = '\0'; 
        } else {
            strncpy(fields[*num_fields_read], token, MAX_STRING -1);
            fields[*num_fields_read][MAX_STRING-1] = '\0';
        }

        if (strcmp(fields[*num_fields_read], "-") == 0 && strlen(fields[*num_fields_read])==1) {
            fields[*num_fields_read][0] = '\0';
        }
        (*num_fields_read)++;
        token = strtok(NULL, ",");
    }
    return 1;
}

void tulis_csv_string(FILE* f, const char* str) {
    char temp[MAX_CATATAN * 2]; 
    char* t_ptr;
    const char* s_ptr;
    int perlu_kutip = 0;

    if (str == NULL || strlen(str) == 0) {
        fprintf(f, "\"-\""); 
        return;
    }

    // Cek apakah string mengandung karakter yang memerlukan kutip
    s_ptr = str;
    while (*s_ptr) {
        if (*s_ptr == ',' || *s_ptr == '"' || *s_ptr == '\n' || *s_ptr == '\r') {
            perlu_kutip = 1;
            break;
        }
        s_ptr++;
    }

    if (perlu_kutip) {
        t_ptr = temp;
        *t_ptr++ = '"'; 
        s_ptr = str;
        while(*s_ptr) {
            if (*s_ptr == '"') {
                *t_ptr++ = '"';
            }
            *t_ptr++ = *s_ptr++;
            if (t_ptr >= temp + sizeof(temp) - 2) break; 
        }
        *t_ptr++ = '"'; 
        *t_ptr = '\0';
        fprintf(f, "%s", temp);
    } else {
        fprintf(f, "%s", str); 
    }
}

void log_aksi_ke_csv(const char* patient_id, const char* aksi, const char* detail_perubahan) {
    FILE* file;
    char timestamp[MAX_STRING];
    long size;

    file = fopen(HISTORY_UPDATES_CSV_FILE, "a");
    if (file == NULL) {
        perror("Gagal membuka file histori");
        return;
    }

    get_current_datetime_str(timestamp, sizeof(timestamp)); 

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    if (size == 0) {
        fprintf(file, "Timestamp,PatientID,Aksi,DetailPerubahan\n");
    }

    tulis_csv_string(file, timestamp); fprintf(file, ",");
    tulis_csv_string(file, patient_id); fprintf(file, ",");
    tulis_csv_string(file, aksi); fprintf(file, ",");
    tulis_csv_string(file, detail_perubahan);
    fprintf(file, "\n");

    fclose(file);
}

void simpan_pasien_ke_csv() {
    FILE* file;
    int i, j, k_loop;

    file = fopen(PATIENT_CSV_FILE, "w");
    if (file == NULL) {
        perror("Gagal membuka file pasien untuk disimpan");
        return;
    }

    fprintf(file, "PatientID,Nama,NIKIDMedis,Usia,JenisKelamin,TglDiagnosis,DokterPJ,Perawat,Konselor,Password,StatusFungsional,Hamil,");
    fprintf(file, "BB,TB,CD4Awal,VLAwal,HbAwal,KlirensKreatininAwal,HBsAg,TesTB,PernahARVSblm,DetailARVSblm,LiniTerapiSblm,TesKehamilanPositif,GulaDarah,");
    for (i = 0; i < MAX_RIWAYAT_KESEHATAN; i++) {
        fprintf(file, "NamaPenyakit%d,PeriodePenyakit%d,", i + 1, i + 1);
    }
    fprintf(file, "JumlahRiwayatKesehatan,");
    fprintf(file, "StatusARVAktual,NamaRegimen,");
    for (i = 0; i < MAX_OBAT_KOMPONEN; i++) {
        fprintf(file, "KompObat%d,DosisKomp%d,", i + 1, i + 1);
    }
    fprintf(file, "DosisRegimenUmum,TglMulaiARTLiniIni,JmlObatAktif,");
    fprintf(file, "SkorWaktuKepatuhan,SkorJumlahObatKepatuhan,SkorDosisKepatuhan,TotalKepatuhanPersen,TglEvalKepatuhan,TglKonsumsiDinilai,TerlambatHariKonsumsi,JmlObatTidakDikonsumsi,");
    fprintf(file, "CD4Terkini,VLTerkini,TglCekLabTerkini,HbTerkini,KreatininTerkini,");
    fprintf(file, "TglCekLabBerikutnya,CatatanPemantauan,");
    fprintf(file, "CatatanKlinisTambahan,CatatanTambahanUmum\n");

    for (i = 0; i < jumlah_pasien_saat_ini; i++) {
        Pasien p_val = daftar_pasien[i];
        tulis_csv_string(file, p_val.patient_id); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.nama); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.nik_id_medis); fprintf(file, ",");
        fprintf(file, "%d,", p_val.demografi.usia);
        fprintf(file, "%d,", p_val.demografi.jenis_kelamin);
        tulis_csv_string(file, p_val.demografi.tanggal_diagnosis); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.dokter_penanggung_jawab); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.perawat); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.konselor); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.password); fprintf(file, ",");
        fprintf(file, "%d,", p_val.demografi.status_fungsional);
        fprintf(file, "%d,", p_val.demografi.hamil); fprintf(file, ",");

        fprintf(file, "%.1f,%.1f,%.2f,%d,%.2f,%.2f,%d,%d,%d,",
                p_val.klinis_awal.berat_badan, p_val.klinis_awal.tinggi_badan,
                p_val.klinis_awal.cd4_awal, p_val.klinis_awal.viral_load_awal,
                p_val.klinis_awal.hb_awal, p_val.klinis_awal.klirens_kreatinin_awal,
                p_val.klinis_awal.hbsag, p_val.klinis_awal.tes_tb, p_val.klinis_awal.pernah_arv_sebelumnya);
        tulis_csv_string(file, p_val.klinis_awal.detail_arv_sebelumnya); fprintf(file, ",");
        fprintf(file, "%d,%d,%.2f,", p_val.klinis_awal.lini_terapi_sebelumnya,
                p_val.klinis_awal.tes_kehamilan_positif, p_val.klinis_awal.gula_darah);

        for (j = 0; j < MAX_RIWAYAT_KESEHATAN; j++) {
            if (j < p_val.jumlah_riwayat_kesehatan) {
                tulis_csv_string(file, p_val.riwayat_kesehatan[j].nama_penyakit); fprintf(file, ",");
                tulis_csv_string(file, p_val.riwayat_kesehatan[j].periode_penyakit); fprintf(file, ",");
            } else {
                tulis_csv_string(file, "-"); fprintf(file, ",");
                tulis_csv_string(file, "-"); fprintf(file, ",");
            }
        }
        fprintf(file, "%d,", p_val.jumlah_riwayat_kesehatan);

        fprintf(file, "%d,", p_val.terapi_arv.status_arv_aktual);
        tulis_csv_string(file, p_val.terapi_arv.nama_regimen); fprintf(file, ",");
        for (k_loop = 0; k_loop < MAX_OBAT_KOMPONEN; k_loop++) {
            tulis_csv_string(file, p_val.terapi_arv.komponen_obat[k_loop]); fprintf(file, ",");
            tulis_csv_string(file, p_val.terapi_arv.dosis_komponen[k_loop]); fprintf(file, ",");
        }
        tulis_csv_string(file, p_val.terapi_arv.dosis_regimen_umum); fprintf(file, ",");
        tulis_csv_string(file, p_val.terapi_arv.tanggal_mulai_art_lini_ini); fprintf(file, ",");
        fprintf(file, "%d,", p_val.terapi_arv.jumlah_obat_aktif);

        fprintf(file, "%d,%d,%d,%d,",
                p_val.kepatuhan.skor_ketepatan_waktu, p_val.kepatuhan.skor_jumlah_obat_diminum,
                p_val.kepatuhan.skor_dosis_obat, p_val.kepatuhan.total_kepatuhan_persen);
        tulis_csv_string(file, p_val.kepatuhan.tanggal_evaluasi_kepatuhan); fprintf(file, ",");
        tulis_csv_string(file, p_val.kepatuhan.tanggal_konsumsi_dinilai); fprintf(file, ",");
        fprintf(file, "%d,", p_val.kepatuhan.terlambat_hari_konsumsi);
        fprintf(file, "%d,", p_val.kepatuhan.jumlah_obat_tidak_dikonsumsi);


        fprintf(file, "%.2f,%d,", p_val.cd4_terkini, p_val.viral_load_terkini);
        tulis_csv_string(file, p_val.tanggal_cek_lab_terkini); fprintf(file, ",");
        fprintf(file, "%.2f,%.2f,", p_val.hb_terkini, p_val.kreatinin_terkini);

        tulis_csv_string(file, p_val.jadwal_pemantauan.tanggal_cek_lab_berikutnya); fprintf(file, ",");
        tulis_csv_string(file, p_val.jadwal_pemantauan.catatan_pemantauan); fprintf(file, ",");

        tulis_csv_string(file, p_val.catatan_klinis_tambahan); fprintf(file, ",");
        tulis_csv_string(file, p_val.catatan_tambahan_umum);
        fprintf(file, "\n");
    }
    fclose(file);
}

void muat_pasien_dari_csv() {
    FILE* file;
    char header_line[4096];
    char fields[200][MAX_STRING];
    int num_fields_read_from_line;
    Pasien* p_ptr;
    int field_idx;
    int i_loop, j_loop;

    const int TOTAL_EXPECTED_FIELDS = 12 + 13 + (MAX_RIWAYAT_KESEHATAN * 2) + 1 + 2 + (MAX_OBAT_KOMPONEN * 2) + 3 + 8 + 5 + 2 + 1 + 1;

    file = fopen(PATIENT_CSV_FILE, "r");
    if (file == NULL) {
        printf("Info: File data pasien (%s) tidak ditemukan. Memulai dengan data kosong.\n", PATIENT_CSV_FILE);
        jumlah_pasien_saat_ini = 0;
        return;
    }

    if (fgets(header_line, sizeof(header_line), file) == NULL) {
        printf("Info: File pasien kosong atau gagal baca header.\n");
        fclose(file);
        jumlah_pasien_saat_ini = 0;
        return;
    }

    jumlah_pasien_saat_ini = 0;

    while (jumlah_pasien_saat_ini < MAX_PATIENTS && baca_csv_line(file, fields, 200, &num_fields_read_from_line)) {
        if (num_fields_read_from_line < TOTAL_EXPECTED_FIELDS) {
            continue;
        }
        p_ptr = &daftar_pasien[jumlah_pasien_saat_ini];
        memset(p_ptr, 0, sizeof(Pasien));
        field_idx = 0;

        strncpy(p_ptr->patient_id, fields[field_idx++], MAX_STRING - 1);p_ptr->patient_id[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.nama, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.nama[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.nik_id_medis, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.nik_id_medis[MAX_STRING-1] = '\0';
        p_ptr->demografi.usia = atoi(fields[field_idx++]);
        p_ptr->demografi.jenis_kelamin = (JenisKelamin)atoi(fields[field_idx++]);
        strncpy(p_ptr->demografi.tanggal_diagnosis, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.tanggal_diagnosis[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.dokter_penanggung_jawab, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.dokter_penanggung_jawab[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.perawat, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.perawat[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.konselor, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.konselor[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.password, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.password[MAX_STRING-1] = '\0';
        p_ptr->demografi.status_fungsional = (StatusFungsional)atoi(fields[field_idx++]);
        p_ptr->demografi.hamil = atoi(fields[field_idx++]);

        p_ptr->klinis_awal.berat_badan = atof(fields[field_idx++]);
        p_ptr->klinis_awal.tinggi_badan = atof(fields[field_idx++]);
        p_ptr->klinis_awal.cd4_awal = atof(fields[field_idx++]);
        p_ptr->klinis_awal.viral_load_awal = atoi(fields[field_idx++]);
        p_ptr->klinis_awal.hb_awal = atof(fields[field_idx++]);
        p_ptr->klinis_awal.klirens_kreatinin_awal = atof(fields[field_idx++]);
        p_ptr->klinis_awal.hbsag = (StatusHBsAg)atoi(fields[field_idx++]);
        p_ptr->klinis_awal.tes_tb = (StatusTesTB)atoi(fields[field_idx++]);
        p_ptr->klinis_awal.pernah_arv_sebelumnya = atoi(fields[field_idx++]);
        strncpy(p_ptr->klinis_awal.detail_arv_sebelumnya, fields[field_idx++], MAX_STRING - 1);p_ptr->klinis_awal.detail_arv_sebelumnya[MAX_STRING-1] = '\0';
        p_ptr->klinis_awal.lini_terapi_sebelumnya = (StatusTerapiARV)atoi(fields[field_idx++]);
        p_ptr->klinis_awal.tes_kehamilan_positif = atoi(fields[field_idx++]);
        p_ptr->klinis_awal.gula_darah = atof(fields[field_idx++]);

        for (i_loop = 0; i_loop < MAX_RIWAYAT_KESEHATAN; i_loop++) {
            strncpy(p_ptr->riwayat_kesehatan[i_loop].nama_penyakit, fields[field_idx++], MAX_STRING - 1);p_ptr->riwayat_kesehatan[i_loop].nama_penyakit[MAX_STRING-1] = '\0';
            strncpy(p_ptr->riwayat_kesehatan[i_loop].periode_penyakit, fields[field_idx++], MAX_STRING - 1);p_ptr->riwayat_kesehatan[i_loop].periode_penyakit[MAX_STRING-1] = '\0';
        }
        p_ptr->jumlah_riwayat_kesehatan = atoi(fields[field_idx++]);

        p_ptr->terapi_arv.status_arv_aktual = (StatusTerapiARV)atoi(fields[field_idx++]);
        strncpy(p_ptr->terapi_arv.nama_regimen, fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.nama_regimen[MAX_STRING-1] = '\0';
        for (j_loop = 0; j_loop < MAX_OBAT_KOMPONEN; j_loop++) {
            strncpy(p_ptr->terapi_arv.komponen_obat[j_loop], fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.komponen_obat[j_loop][MAX_STRING-1] = '\0';
            strncpy(p_ptr->terapi_arv.dosis_komponen[j_loop], fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.dosis_komponen[j_loop][MAX_STRING-1] = '\0';
        }
        strncpy(p_ptr->terapi_arv.dosis_regimen_umum, fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.dosis_regimen_umum[MAX_STRING-1] = '\0';
        strncpy(p_ptr->terapi_arv.tanggal_mulai_art_lini_ini, fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.tanggal_mulai_art_lini_ini[MAX_STRING-1] = '\0';
        p_ptr->terapi_arv.jumlah_obat_aktif = atoi(fields[field_idx++]);

        p_ptr->kepatuhan.skor_ketepatan_waktu = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.skor_jumlah_obat_diminum = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.skor_dosis_obat = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.total_kepatuhan_persen = atoi(fields[field_idx++]);
        strncpy(p_ptr->kepatuhan.tanggal_evaluasi_kepatuhan, fields[field_idx++], MAX_STRING - 1);p_ptr->kepatuhan.tanggal_evaluasi_kepatuhan[MAX_STRING-1] = '\0';
        strncpy(p_ptr->kepatuhan.tanggal_konsumsi_dinilai, fields[field_idx++], MAX_STRING - 1);p_ptr->kepatuhan.tanggal_konsumsi_dinilai[MAX_STRING-1] = '\0';
        p_ptr->kepatuhan.terlambat_hari_konsumsi = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.jumlah_obat_tidak_dikonsumsi = atoi(fields[field_idx++]);

        p_ptr->cd4_terkini = atof(fields[field_idx++]);
        p_ptr->viral_load_terkini = atoi(fields[field_idx++]);
        strncpy(p_ptr->tanggal_cek_lab_terkini, fields[field_idx++], MAX_STRING - 1);p_ptr->tanggal_cek_lab_terkini[MAX_STRING-1] = '\0';
        p_ptr->hb_terkini = atof(fields[field_idx++]);
        p_ptr->kreatinin_terkini = atof(fields[field_idx++]);

        strncpy(p_ptr->jadwal_pemantauan.tanggal_cek_lab_berikutnya, fields[field_idx++], MAX_STRING - 1);p_ptr->jadwal_pemantauan.tanggal_cek_lab_berikutnya[MAX_STRING-1] = '\0';
        strncpy(p_ptr->jadwal_pemantauan.catatan_pemantauan, fields[field_idx++], MAX_CATATAN - 1);p_ptr->jadwal_pemantauan.catatan_pemantauan[MAX_CATATAN-1] = '\0';

        strncpy(p_ptr->catatan_klinis_tambahan, fields[field_idx++], MAX_CATATAN - 1);p_ptr->catatan_klinis_tambahan[MAX_CATATAN-1] = '\0';
        strncpy(p_ptr->catatan_tambahan_umum, fields[field_idx++], MAX_CATATAN - 1);p_ptr->catatan_tambahan_umum[MAX_CATATAN-1] = '\0';

        jumlah_pasien_saat_ini++;
    }
    fclose(file);
    if (jumlah_pasien_saat_ini > 0) {
        printf("Data pasien berhasil dimuat dari %s (%d pasien)\n", PATIENT_CSV_FILE, jumlah_pasien_saat_ini);
    }
}

void simpan_riwayat_hapus_ke_csv() {
    FILE* file;
    int i_loop, j_loop, k_loop;

    file = fopen(DELETED_PATIENTS_CSV_FILE, "w");
    if (file == NULL) {
        perror("Gagal membuka file riwayat hapus untuk disimpan");
        return;
    }

    fprintf(file, "PatientID,Nama,NIKIDMedis,Usia,JenisKelamin,TglDiagnosis,DokterPJ,Perawat,Konselor,Password,StatusFungsional,Hamil,");
    fprintf(file, "BB,TB,CD4Awal,VLAwal,HbAwal,KlirensKreatininAwal,HBsAg,TesTB,PernahARVSblm,DetailARVSblm,LiniTerapiSblm,TesKehamilanPositif,GulaDarah,");
    for (i_loop = 0; i_loop < MAX_RIWAYAT_KESEHATAN; i_loop++) {
        fprintf(file, "NamaPenyakit%d,PeriodePenyakit%d,", i_loop + 1, i_loop + 1);
    }
    fprintf(file, "JumlahRiwayatKesehatan,");
    fprintf(file, "StatusARVAktual,NamaRegimen,");
    for (i_loop = 0; i_loop < MAX_OBAT_KOMPONEN; i_loop++) {
        fprintf(file, "KompObat%d,DosisKomp%d,", i_loop + 1, i_loop + 1);
    }
    fprintf(file, "DosisRegimenUmum,TglMulaiARTLiniIni,JmlObatAktif,");
    fprintf(file, "SkorWaktuKepatuhan,SkorJumlahObatKepatuhan,SkorDosisKepatuhan,TotalKepatuhanPersen,TglEvalKepatuhan,TglKonsumsiDinilai,TerlambatHariKonsumsi,JmlObatTidakDikonsumsi,");
    fprintf(file, "CD4Terkini,VLTerkini,TglCekLabTerkini,HbTerkini,KreatininTerkini,");
    fprintf(file, "TglCekLabBerikutnya,CatatanPemantauan,");
    fprintf(file, "CatatanKlinisTambahan,CatatanTambahanUmum,");
    fprintf(file, "TanggalHapus,AlasanHapus\n");

    for (i_loop = 0; i_loop < jumlah_riwayat_hapus_saat_ini; i_loop++) {
        Pasien p_val = daftar_riwayat_hapus[i_loop].pasien_yang_dihapus;
        tulis_csv_string(file, p_val.patient_id); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.nama); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.nik_id_medis); fprintf(file, ",");
        fprintf(file, "%d,", p_val.demografi.usia);
        fprintf(file, "%d,", p_val.demografi.jenis_kelamin);
        tulis_csv_string(file, p_val.demografi.tanggal_diagnosis); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.dokter_penanggung_jawab); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.perawat); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.konselor); fprintf(file, ",");
        tulis_csv_string(file, p_val.demografi.password); fprintf(file, ",");
        fprintf(file, "%d,", p_val.demografi.status_fungsional);
        fprintf(file, "%d,", p_val.demografi.hamil); fprintf(file, ",");

        fprintf(file, "%.1f,%.1f,%.2f,%d,%.2f,%.2f,%d,%d,%d,",
                p_val.klinis_awal.berat_badan, p_val.klinis_awal.tinggi_badan,
                p_val.klinis_awal.cd4_awal, p_val.klinis_awal.viral_load_awal,
                p_val.klinis_awal.hb_awal, p_val.klinis_awal.klirens_kreatinin_awal,
                p_val.klinis_awal.hbsag, p_val.klinis_awal.tes_tb, p_val.klinis_awal.pernah_arv_sebelumnya);
        tulis_csv_string(file, p_val.klinis_awal.detail_arv_sebelumnya); fprintf(file, ",");
        fprintf(file, "%d,%d,%.2f,", p_val.klinis_awal.lini_terapi_sebelumnya,
                p_val.klinis_awal.tes_kehamilan_positif, p_val.klinis_awal.gula_darah);

        for (j_loop = 0; j_loop < MAX_RIWAYAT_KESEHATAN; j_loop++) {
            if (j_loop < p_val.jumlah_riwayat_kesehatan) {
                tulis_csv_string(file, p_val.riwayat_kesehatan[j_loop].nama_penyakit); fprintf(file, ",");
                tulis_csv_string(file, p_val.riwayat_kesehatan[j_loop].periode_penyakit); fprintf(file, ",");
            } else {
                tulis_csv_string(file, "-"); fprintf(file, ",");
                tulis_csv_string(file, "-"); fprintf(file, ",");
            }
        }
        fprintf(file, "%d,", p_val.jumlah_riwayat_kesehatan);

        fprintf(file, "%d,", p_val.terapi_arv.status_arv_aktual);
        tulis_csv_string(file, p_val.terapi_arv.nama_regimen); fprintf(file, ",");
        for (k_loop = 0; k_loop < MAX_OBAT_KOMPONEN; k_loop++) {
            tulis_csv_string(file, p_val.terapi_arv.komponen_obat[k_loop]); fprintf(file, ",");
            tulis_csv_string(file, p_val.terapi_arv.dosis_komponen[k_loop]); fprintf(file, ",");
        }
        tulis_csv_string(file, p_val.terapi_arv.dosis_regimen_umum); fprintf(file, ",");
        tulis_csv_string(file, p_val.terapi_arv.tanggal_mulai_art_lini_ini); fprintf(file, ",");
        fprintf(file, "%d,", p_val.terapi_arv.jumlah_obat_aktif);

        fprintf(file, "%d,%d,%d,%d,",
                p_val.kepatuhan.skor_ketepatan_waktu, p_val.kepatuhan.skor_jumlah_obat_diminum,
                p_val.kepatuhan.skor_dosis_obat, p_val.kepatuhan.total_kepatuhan_persen);
        tulis_csv_string(file, p_val.kepatuhan.tanggal_evaluasi_kepatuhan); fprintf(file, ",");
        tulis_csv_string(file, p_val.kepatuhan.tanggal_konsumsi_dinilai); fprintf(file, ",");
        fprintf(file, "%d,", p_val.kepatuhan.terlambat_hari_konsumsi);
        fprintf(file, "%d,", p_val.kepatuhan.jumlah_obat_tidak_dikonsumsi);

        fprintf(file, "%.2f,%d,", p_val.cd4_terkini, p_val.viral_load_terkini);
        tulis_csv_string(file, p_val.tanggal_cek_lab_terkini); fprintf(file, ",");
        fprintf(file, "%.2f,%.2f,", p_val.hb_terkini, p_val.kreatinin_terkini);

        tulis_csv_string(file, p_val.jadwal_pemantauan.tanggal_cek_lab_berikutnya); fprintf(file, ",");
        tulis_csv_string(file, p_val.jadwal_pemantauan.catatan_pemantauan); fprintf(file, ",");
        tulis_csv_string(file, p_val.catatan_klinis_tambahan); fprintf(file, ",");
        tulis_csv_string(file, p_val.catatan_tambahan_umum); fprintf(file, ",");

        tulis_csv_string(file, daftar_riwayat_hapus[i_loop].tanggal_hapus); fprintf(file, ",");
        tulis_csv_string(file, daftar_riwayat_hapus[i_loop].alasan_hapus);
        fprintf(file, "\n");
    }
    fclose(file);
}

void muat_riwayat_hapus_dari_csv() {
    FILE* file;
    char header_line[4096];
    RiwayatHapusPasien* rh_ptr;
    Pasien* p_ptr;
    char fields[200][MAX_STRING];
    int num_fields_read_from_line;
    int field_idx;
    int i_loop, j_loop;

    const int TOTAL_EXPECTED_FIELDS_DELETED = 12 + 13 + (MAX_RIWAYAT_KESEHATAN * 2) + 1 + 2 + (MAX_OBAT_KOMPONEN * 2) + 3 + 8 + 5 + 2 + 1 + 1 + 2;

    file = fopen(DELETED_PATIENTS_CSV_FILE, "r");
    if (file == NULL) {
        jumlah_riwayat_hapus_saat_ini = 0;
        return;
    }

    if (fgets(header_line, sizeof(header_line), file) == NULL) {
        fclose(file);
        jumlah_riwayat_hapus_saat_ini = 0;
        return;
    }

    jumlah_riwayat_hapus_saat_ini = 0;

    while (jumlah_riwayat_hapus_saat_ini < MAX_PATIENTS && baca_csv_line(file, fields, 200, &num_fields_read_from_line)) {
        if (num_fields_read_from_line < TOTAL_EXPECTED_FIELDS_DELETED) {
            continue;
        }
        rh_ptr = &daftar_riwayat_hapus[jumlah_riwayat_hapus_saat_ini];
        p_ptr = &rh_ptr->pasien_yang_dihapus;
        memset(p_ptr, 0, sizeof(Pasien)); // Inisialisasi pasien_yang_dihapus

        field_idx = 0;

        strncpy(p_ptr->patient_id, fields[field_idx++], MAX_STRING - 1);p_ptr->patient_id[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.nama, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.nama[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.nik_id_medis, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.nik_id_medis[MAX_STRING-1] = '\0';
        p_ptr->demografi.usia = atoi(fields[field_idx++]);
        p_ptr->demografi.jenis_kelamin = (JenisKelamin)atoi(fields[field_idx++]);
        strncpy(p_ptr->demografi.tanggal_diagnosis, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.tanggal_diagnosis[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.dokter_penanggung_jawab, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.dokter_penanggung_jawab[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.perawat, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.perawat[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.konselor, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.konselor[MAX_STRING-1] = '\0';
        strncpy(p_ptr->demografi.password, fields[field_idx++], MAX_STRING - 1);p_ptr->demografi.password[MAX_STRING-1] = '\0';
        p_ptr->demografi.status_fungsional = (StatusFungsional)atoi(fields[field_idx++]);
        p_ptr->demografi.hamil = atoi(fields[field_idx++]);

        p_ptr->klinis_awal.berat_badan = atof(fields[field_idx++]);
        p_ptr->klinis_awal.tinggi_badan = atof(fields[field_idx++]);
        p_ptr->klinis_awal.cd4_awal = atof(fields[field_idx++]);
        p_ptr->klinis_awal.viral_load_awal = atoi(fields[field_idx++]);
        p_ptr->klinis_awal.hb_awal = atof(fields[field_idx++]);
        p_ptr->klinis_awal.klirens_kreatinin_awal = atof(fields[field_idx++]);
        p_ptr->klinis_awal.hbsag = (StatusHBsAg)atoi(fields[field_idx++]);
        p_ptr->klinis_awal.tes_tb = (StatusTesTB)atoi(fields[field_idx++]);
        p_ptr->klinis_awal.pernah_arv_sebelumnya = atoi(fields[field_idx++]);
        strncpy(p_ptr->klinis_awal.detail_arv_sebelumnya, fields[field_idx++], MAX_STRING - 1);p_ptr->klinis_awal.detail_arv_sebelumnya[MAX_STRING-1] = '\0';
        p_ptr->klinis_awal.lini_terapi_sebelumnya = (StatusTerapiARV)atoi(fields[field_idx++]);
        p_ptr->klinis_awal.tes_kehamilan_positif = atoi(fields[field_idx++]);
        p_ptr->klinis_awal.gula_darah = atof(fields[field_idx++]);

        for (i_loop = 0; i_loop < MAX_RIWAYAT_KESEHATAN; i_loop++) {
            strncpy(p_ptr->riwayat_kesehatan[i_loop].nama_penyakit, fields[field_idx++], MAX_STRING - 1);p_ptr->riwayat_kesehatan[i_loop].nama_penyakit[MAX_STRING-1] = '\0';
            strncpy(p_ptr->riwayat_kesehatan[i_loop].periode_penyakit, fields[field_idx++], MAX_STRING - 1);p_ptr->riwayat_kesehatan[i_loop].periode_penyakit[MAX_STRING-1] = '\0';
        }
        p_ptr->jumlah_riwayat_kesehatan = atoi(fields[field_idx++]);

        p_ptr->terapi_arv.status_arv_aktual = (StatusTerapiARV)atoi(fields[field_idx++]);
        strncpy(p_ptr->terapi_arv.nama_regimen, fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.nama_regimen[MAX_STRING-1] = '\0';
        for (j_loop = 0; j_loop < MAX_OBAT_KOMPONEN; j_loop++) {
            strncpy(p_ptr->terapi_arv.komponen_obat[j_loop], fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.komponen_obat[j_loop][MAX_STRING-1] = '\0';
            strncpy(p_ptr->terapi_arv.dosis_komponen[j_loop], fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.dosis_komponen[j_loop][MAX_STRING-1] = '\0';
        }
        strncpy(p_ptr->terapi_arv.dosis_regimen_umum, fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.dosis_regimen_umum[MAX_STRING-1] = '\0';
        strncpy(p_ptr->terapi_arv.tanggal_mulai_art_lini_ini, fields[field_idx++], MAX_STRING - 1);p_ptr->terapi_arv.tanggal_mulai_art_lini_ini[MAX_STRING-1] = '\0';
        p_ptr->terapi_arv.jumlah_obat_aktif = atoi(fields[field_idx++]);

        p_ptr->kepatuhan.skor_ketepatan_waktu = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.skor_jumlah_obat_diminum = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.skor_dosis_obat = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.total_kepatuhan_persen = atoi(fields[field_idx++]);
        strncpy(p_ptr->kepatuhan.tanggal_evaluasi_kepatuhan, fields[field_idx++], MAX_STRING - 1);p_ptr->kepatuhan.tanggal_evaluasi_kepatuhan[MAX_STRING-1] = '\0';
        strncpy(p_ptr->kepatuhan.tanggal_konsumsi_dinilai, fields[field_idx++], MAX_STRING - 1);p_ptr->kepatuhan.tanggal_konsumsi_dinilai[MAX_STRING-1] = '\0';
        p_ptr->kepatuhan.terlambat_hari_konsumsi = atoi(fields[field_idx++]);
        p_ptr->kepatuhan.jumlah_obat_tidak_dikonsumsi = atoi(fields[field_idx++]);

        p_ptr->cd4_terkini = atof(fields[field_idx++]);
        p_ptr->viral_load_terkini = atoi(fields[field_idx++]);
        strncpy(p_ptr->tanggal_cek_lab_terkini, fields[field_idx++], MAX_STRING - 1);p_ptr->tanggal_cek_lab_terkini[MAX_STRING-1] = '\0';
        p_ptr->hb_terkini = atof(fields[field_idx++]);
        p_ptr->kreatinin_terkini = atof(fields[field_idx++]);

        strncpy(p_ptr->jadwal_pemantauan.tanggal_cek_lab_berikutnya, fields[field_idx++], MAX_STRING - 1);p_ptr->jadwal_pemantauan.tanggal_cek_lab_berikutnya[MAX_STRING-1] = '\0';
        strncpy(p_ptr->jadwal_pemantauan.catatan_pemantauan, fields[field_idx++], MAX_CATATAN - 1);p_ptr->jadwal_pemantauan.catatan_pemantauan[MAX_CATATAN-1] = '\0';
        strncpy(p_ptr->catatan_klinis_tambahan, fields[field_idx++], MAX_CATATAN - 1);p_ptr->catatan_klinis_tambahan[MAX_CATATAN-1] = '\0';
        strncpy(p_ptr->catatan_tambahan_umum, fields[field_idx++], MAX_CATATAN - 1);p_ptr->catatan_tambahan_umum[MAX_CATATAN-1] = '\0';

        strncpy(rh_ptr->tanggal_hapus, fields[field_idx++], MAX_STRING - 1);rh_ptr->tanggal_hapus[MAX_STRING - 1] = '\0';
        strncpy(rh_ptr->alasan_hapus, fields[field_idx++], (MAX_STRING * 2) - 1);rh_ptr->alasan_hapus[(MAX_STRING * 2) - 1] = '\0';

        jumlah_riwayat_hapus_saat_ini++;
    }
    fclose(file);
}

void muat_semua_data_dari_csv() {
    muat_pasien_dari_csv();
    muat_riwayat_hapus_dari_csv();
}

void simpan_semua_data_ke_csv() {
    simpan_pasien_ke_csv();
    simpan_riwayat_hapus_ke_csv();
}

void hapus_data_pasien_menu() {
    char id_hapus[MAX_STRING];
    int index_pasien;
    char password[MAX_STRING];
    char konfirmasi;
    char tanggal_hapus_str[MAX_STRING];
    char alasan[MAX_STRING * 2];
    int i_loop;
    const char* status_arv_str;

    if (jumlah_pasien_saat_ini == 0) {
        printf("Belum ada pasien terdaftar.\n");
        return;
    }
    printf("Masukkan Patient ID yang akan dihapus: ");
    get_string_input(NULL, id_hapus, MAX_STRING); 


    index_pasien = -1;
    for (i_loop = 0; i_loop < jumlah_pasien_saat_ini; i_loop++) {
        if (strcmp(daftar_pasien[i_loop].patient_id, id_hapus) == 0) {
            index_pasien = i_loop;
            break;
        }
    }
    if (index_pasien == -1) {
        printf("Pasien dengan ID %s tidak ditemukan.\n", id_hapus);
        return;
    }
    printf("Masukkan Password Pasien: ");
    get_string_input(NULL, password, MAX_STRING);


    if (strcmp(daftar_pasien[index_pasien].demografi.password, password) != 0) {
        printf("Password salah! Penghapusan dibatalkan.\n");
        return;
    }
    printf("\n--- HAPUS PASIEN ---\n");
    printf("Patient ID            : %s\n", daftar_pasien[index_pasien].patient_id);
    printf("Usia                  : %d\n", daftar_pasien[index_pasien].demografi.usia);
    printf("Tanggal Terakhir Cek Lab: %s\n", strlen(daftar_pasien[index_pasien].tanggal_cek_lab_terkini) > 0 ? daftar_pasien[index_pasien].tanggal_cek_lab_terkini : "-");
    switch(daftar_pasien[index_pasien].terapi_arv.status_arv_aktual) {
        case BELUM_TERAPI: status_arv_str = "Belum Terapi"; break;
        case LINI_PERTAMA: status_arv_str = "Lini Pertama"; break;
        case LINI_KEDUA:   status_arv_str = "Lini Kedua";   break;
        case LINI_KETIGA:  status_arv_str = "Lini Ketiga";  break;
        case TERAPI_GAGAL_TERMINAL: status_arv_str = "Gagal Terminal"; break;
        default:           status_arv_str = "Tidak Diketahui"; break;
    }
    printf("Status ARV            : %s\n", status_arv_str);
    printf("Kepatuhan ARV (%%)     : %d\n", daftar_pasien[index_pasien].kepatuhan.total_kepatuhan_persen);
    printf("Alasan Penghapusan Pasien: ");
    get_string_input(NULL, alasan, sizeof(alasan));

    if (strlen(alasan) == 0) {
        strcpy(alasan, "Dihapus melalui menu oleh pengguna.");
    }
    printf("HAPUS PASIEN (Y/N)? ");
    char konfirmasi_str[10];
    get_string_input(NULL, konfirmasi_str, sizeof(konfirmasi_str));
    konfirmasi = konfirmasi_str[0];

    if (konfirmasi != 'Y' && konfirmasi != 'y') {
        printf("Penghapusan dibatalkan.\n");
        return;
    }
    get_current_date_str(tanggal_hapus_str, sizeof(tanggal_hapus_str)); 
    hapus_pasien_secara_internal(&daftar_pasien[index_pasien], alasan, tanggal_hapus_str);
}

void hapus_pasien_secara_internal(Pasien* p_target, const char* alasan_penghapusan, const char* tanggal_penghapusan_str) {
    int index_pasien = -1;
    char detail_log[MAX_CATATAN * 2]; 
    int i_loop;

    if (p_target == NULL) return;
    for (i_loop = 0; i_loop < jumlah_pasien_saat_ini; i_loop++) {
        if (&daftar_pasien[i_loop] == p_target || strcmp(daftar_pasien[i_loop].patient_id, p_target->patient_id) == 0) {
            index_pasien = i_loop;
            break;
        }
    }
    if (index_pasien == -1) {
        printf("Error internal: Pasien target untuk dihapus tidak ditemukan di daftar aktif.\n");
        return;
    }
    if (jumlah_riwayat_hapus_saat_ini < MAX_PATIENTS) {
        daftar_riwayat_hapus[jumlah_riwayat_hapus_saat_ini].pasien_yang_dihapus = *p_target;
        strncpy(daftar_riwayat_hapus[jumlah_riwayat_hapus_saat_ini].tanggal_hapus, tanggal_penghapusan_str, MAX_STRING-1);
        daftar_riwayat_hapus[jumlah_riwayat_hapus_saat_ini].tanggal_hapus[MAX_STRING-1] = '\0';
        strncpy(daftar_riwayat_hapus[jumlah_riwayat_hapus_saat_ini].alasan_hapus, alasan_penghapusan, (MAX_STRING*2)-1);
        daftar_riwayat_hapus[jumlah_riwayat_hapus_saat_ini].alasan_hapus[(MAX_STRING*2)-1] = '\0';
        jumlah_riwayat_hapus_saat_ini++;
        simpan_riwayat_hapus_ke_csv();
    } else {
        printf("Peringatan: Daftar riwayat hapus penuh. Tidak dapat menambah.\n");
    }
    snprintf(detail_log, sizeof(detail_log), "Pasien dihapus. ID: %s, Nama: %s, Alasan: %s, Tgl Hapus: %s",
            p_target->patient_id, p_target->demografi.nama, alasan_penghapusan, tanggal_penghapusan_str);
    log_aksi_ke_csv(p_target->patient_id, "HAPUS_PASIEN", detail_log);
    for (i_loop = index_pasien; i_loop < jumlah_pasien_saat_ini - 1; i_loop++) {
        daftar_pasien[i_loop] = daftar_pasien[i_loop + 1];
    }
    jumlah_pasien_saat_ini--;
    simpan_pasien_ke_csv();
    printf("Pasien %s (%s) berhasil dihapus dari daftar aktif dan dicatat riwayatnya.\n", p_target->patient_id, p_target->demografi.nama);
}

void lihat_riwayat_hapus_dari_memori() {
    int i_loop;
    const char* status_arv_str;

    if (jumlah_riwayat_hapus_saat_ini == 0) {
        printf("Belum ada pasien yang dihapus dalam sesi ini atau dari file.\n");
        return;
    }
    printf("\n--- LIHAT RIWAYAT PASIEN YANG DIHAPUS ---\n");
    printf("%-12s %-20s %-5s %-12s %-15s %-10s %-12s %s\n",
        "Patient ID", "Nama", "Usia", "Tgl Cek Lab", "Status ARV", "Kepatuhan", "Tgl Hapus", "Alasan Hapus");
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    for (i_loop = 0; i_loop < jumlah_riwayat_hapus_saat_ini; i_loop++) {
        RiwayatHapusPasien rh = daftar_riwayat_hapus[i_loop];
        Pasien p_val = rh.pasien_yang_dihapus;
        switch(p_val.terapi_arv.status_arv_aktual) {
            case BELUM_TERAPI: status_arv_str = "Belum"; break;
            case LINI_PERTAMA: status_arv_str = "Lini 1"; break;
            case LINI_KEDUA:   status_arv_str = "Lini 2";   break;
            case LINI_KETIGA:  status_arv_str = "Lini 3";  break;
            case TERAPI_GAGAL_TERMINAL: status_arv_str = "Gagal"; break;
            default:           status_arv_str = "N/A"; break;
        }
        printf("%-12s %-20.20s %-5d %-12.12s %-15.15s %-10d %-12.12s %s\n",
            p_val.patient_id, p_val.demografi.nama, p_val.demografi.usia,
            strlen(p_val.tanggal_cek_lab_terkini) > 0 ? p_val.tanggal_cek_lab_terkini : "-",
            status_arv_str, p_val.kepatuhan.total_kepatuhan_persen,
            rh.tanggal_hapus, rh.alasan_hapus);
    }
    printf("----------------------------------------------------------------------------------------------------------------------\n");
}

void restore_pasien_dari_memori() {
    char id_restore[MAX_STRING];
    char password[MAX_STRING];
    char konfirmasi_str[10];
    int index_hapus = -1;
    int i_loop;
    char detail_log[MAX_CATATAN * 2]; 
    if (jumlah_riwayat_hapus_saat_ini == 0) {
        printf("Tidak ada pasien di riwayat hapus untuk direstore.\n");
        return;
    }
    printf("\n--- RESTORE DATA PASIEN ---\n");
    printf("Masukkan Patient ID yang akan direstore: ");
    get_string_input(NULL, id_restore, MAX_STRING);


    for (i_loop = 0; i_loop < jumlah_riwayat_hapus_saat_ini; i_loop++) {
        if (strcmp(daftar_riwayat_hapus[i_loop].pasien_yang_dihapus.patient_id, id_restore) == 0) {
            index_hapus = i_loop;
            break;
        }
    }
    if (index_hapus == -1) {
        printf("Pasien dengan ID %s tidak ditemukan di riwayat hapus.\n", id_restore);
        return;
    }
    printf("Masukkan Password Pasien %s: ", id_restore);
    get_string_input(NULL, password, MAX_STRING);


    if (strcmp(daftar_riwayat_hapus[index_hapus].pasien_yang_dihapus.demografi.password, password) != 0) {
        printf("Password salah. Restore dibatalkan.\n");
        return;
    }
    printf("RESTORE DATA PASIEN (Y/N)? ");
    get_string_input(NULL, konfirmasi_str, sizeof(konfirmasi_str));


    if (konfirmasi_str[0] == 'Y' || konfirmasi_str[0] == 'y') {
        if (jumlah_pasien_saat_ini >= MAX_PATIENTS) {
            printf("Kapasitas pasien aktif penuh. Tidak dapat merestore.\n");
            return;
        }
        daftar_pasien[jumlah_pasien_saat_ini] = daftar_riwayat_hapus[index_hapus].pasien_yang_dihapus;
        jumlah_pasien_saat_ini++;
        snprintf(detail_log, sizeof(detail_log), "Pasien direstore dari riwayat hapus. ID: %s, Nama: %s",
                daftar_riwayat_hapus[index_hapus].pasien_yang_dihapus.patient_id,
                daftar_riwayat_hapus[index_hapus].pasien_yang_dihapus.demografi.nama);
        log_aksi_ke_csv(daftar_riwayat_hapus[index_hapus].pasien_yang_dihapus.patient_id, "RESTORE_PASIEN", detail_log);
        for (i_loop = index_hapus; i_loop < jumlah_riwayat_hapus_saat_ini - 1; i_loop++) {
            daftar_riwayat_hapus[i_loop] = daftar_riwayat_hapus[i_loop + 1];
        }
        jumlah_riwayat_hapus_saat_ini--;
        printf("Pasien %s berhasil direstore ke daftar aktif.\n", id_restore);
        simpan_semua_data_ke_csv();
    } else {
        printf("Restore dibatalkan.\n");
    }
}

void menu_penghapusan_pasien_lanjutan() {
    int pilihan;
    do {
        printf("\n--- DASHBOARD PASIEN DIHAPUS ---\n");
        printf("1. Hapus Pasien\n");
        printf("2. Lihat Riwayat Pasien yang Dihapus\n");
        printf("3. Restore Data Pasien\n");
        printf("0. Kembali ke Menu Utama\n");
        pilihan = get_int_input("Pilihan Anda");


        switch (pilihan) {
            case 1: hapus_data_pasien_menu(); break;
            case 2: lihat_riwayat_hapus_dari_memori(); break;
            case 3: restore_pasien_dari_memori(); break;
            case 0: printf("Kembali ke menu utama...\n"); break;
            default: printf("Pilihan tidak valid. Silakan coba lagi.\n");
        }
    } while (pilihan != 0);
}
