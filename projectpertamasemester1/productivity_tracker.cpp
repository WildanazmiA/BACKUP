#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

// Fungsi ambil tanggal otomatis (format YYYY-MM-DD)
string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;

    string date = to_string(year) + "-" +
        (month < 10 ? "0" + to_string(month) : to_string(month)) + "-" +
        (day < 10 ? "0" + to_string(day) : to_string(day));
    return date;
}

// Fungsi pilih kategori dari daftar tetap
string pilihKategori() {
    int pilihan;
    string kategoriList[5] = {"Belajar", "Pekerjaan", "Kesehatan", "Hiburan", "Pribadi"};

    cout << "\nPilih kategori:\n";
    for (int i = 0; i < 5; i++) {
        cout << i + 1 << ". " << kategoriList[i] << endl;
    }

    do {
        cout << "Masukkan nomor kategori (1-5): ";
        cin >> pilihan;
        if (pilihan < 1 || pilihan > 5) {
            cout << "Pilihan tidak valid. Coba lagi.\n";
        }
    } while (pilihan < 1 || pilihan > 5);

    cin.ignore(); // bersihkan newline dari buffer
    return kategoriList[pilihan - 1];
}

int main() {
    ofstream file("productivity_tracker.csv", ios::app);
    if (!file.is_open()) {
        cout << "Gagal membuka file CSV." << endl;
        return 1;
    }

    // Tambahkan header jika file kosong
    ifstream check("productivity_tracker.csv");
    check.seekg(0, ios::end);
    if (check.tellg() == 0) {
        file << "Tanggal,Aktivitas,Durasi (jam),Kategori,Skor Produktivitas,Catatan\n";
    }
    check.close();

    string aktivitas, catatan;
    double jamMulai, jamSelesai;
    int skor;

    cout << "=== PRODUCTIVITY TRACKER ===" << endl;
    string tanggal = getCurrentDate();
    cout << "Tanggal otomatis: " << tanggal << endl;

    cout << "Masukkan aktivitas: ";
    getline(cin, aktivitas);

    cout << "Masukkan jam mulai (contoh: 8.5 untuk 08:30): ";
    cin >> jamMulai;
    cout << "Masukkan jam selesai: ";
    cin >> jamSelesai;

    double durasi = jamSelesai - jamMulai;
    cin.ignore();

    string kategori = pilihKategori();

    cout << "Skor produktivitas (1-10): ";
    cin >> skor;
    cin.ignore();

    cout << "Catatan tambahan: ";
    getline(cin, catatan);

    file << tanggal << "," << aktivitas << "," << durasi << ","
    << kategori << "," << skor << "," << catatan << "\n";

    file.close();

    cout << "\nData berhasil disimpan ke productivity_tracker.csv" << endl;
    return 0;
}