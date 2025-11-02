#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cstdlib>

using namespace std;

// ===== Struktur Data Mahasiswa =====
struct Mahasiswa {
    string nama;
    int jumlahHadir;
    float persentase;
    bool isLulus;
};

// ===== Fungsi Membaca Data dari File CSV =====
void bacaDataDariFile(vector<Mahasiswa> &daftar) {
    ifstream file("rekap.csv");
    if (!file) return;  // Jika file tidak ada, keluar

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        Mahasiswa mhs;
        string tmp;
        size_t pos = 0;

        // Nama
        pos = line.find(',');
        if (pos == string::npos) continue;
        mhs.nama = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Jumlah hadir
        pos = line.find(',');
        if (pos == string::npos) continue;
        tmp = line.substr(0, pos);
        mhs.jumlahHadir = atoi(tmp.c_str());
        line.erase(0, pos + 1);

        // Persentase
        pos = line.find(',');
        if (pos == string::npos) continue;
        tmp = line.substr(0, pos);
        mhs.persentase = atof(tmp.c_str());
        line.erase(0, pos + 1);

        // Status lulus
        mhs.isLulus = (line == "1") ? true : false;

        daftar.push_back(mhs);
    }
    file.close();
}

// ===== Fungsi Menyimpan Semua Data ke File CSV =====
void simpanSemuaData(const vector<Mahasiswa> &daftar) {
    ofstream file("rekap.csv");
    if (!file) return;

    file << "Nama Mahasiswa,Jumlah Hadir,Persentase,Lulus\n";
    for (auto &mhs : daftar) {
        file << mhs.nama << ","
            << mhs.jumlahHadir << ","
            << mhs.persentase << ","
            << (mhs.isLulus ? "1" : "0") << "\n";
    }
    file.close();
}

// ===== Fungsi Login =====
bool login(string &role) {
    string username, password;
    int percobaan = 0;

    while (percobaan < 3) {
        cout << "=== HALAMAN LOGIN ===" << endl;
        cout << "Masukkan Username : "; cin >> username;
        cout << "Masukkan Password : "; cin >> password;

        // cek admin
        if (username == "admin" && password == "12345") {
            cout << "\nLogin berhasil! Selamat datang, Admin.\n\n";
            role = "admin";
            return true;
        }
        // cek user
        else if (username == "user" && password == "111") {
            cout << "\nLogin berhasil! Selamat datang, User.\n\n";
            role = "user";
            return true;
        }
        else {
            cout << "\n[ERROR] Username atau Password salah!\n\n";
            percobaan++;
        }
    }

    cout << "Anda sudah gagal login 3 kali. Program dihentikan.\n";
    return false;
}

// ===== PROGRAM UTAMA =====
int main() {
    vector<Mahasiswa> daftarMahasiswa;
    bacaDataDariFile(daftarMahasiswa);

    while (true) { // supaya bisa balik ke login
        string role = "";
        if (!login(role)) break; // jika gagal login 3x, keluar program

        int pilihan;
        bool logout = false;

        // ===== MENU UTAMA =====
        do {
            cout << "=== MENU UTAMA (" << role << ") ===\n";
            cout << "1. Input Data Presensi\n";
            cout << "2. Lihat Rekap Presensi\n";
            if (role == "admin") {
                cout << "3. Edit Data Mahasiswa\n";
                cout << "4. Hapus Data Mahasiswa\n";
                cout << "5. Logout\n";
                cout << "6. Keluar Program\n";
            } else {
                cout << "3. Logout\n";
                cout << "4. Keluar Program\n";
            }
            cout << "Pilih menu: ";
            cin >> pilihan;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (pilihan == 1) {
                // INPUT
                Mahasiswa mhs;
                cout << "\n=== Input Data Kehadiran Mahasiswa ===\n";
                cout << "Nama Mahasiswa        : ";
                getline(cin, mhs.nama);

                cout << "Jumlah Hadir (0-16)   : ";
                cin >> mhs.jumlahHadir;
                while (mhs.jumlahHadir < 0 || mhs.jumlahHadir > 16) {
                    cout << "[ERROR] Jumlah hadir hanya boleh 0-16.\n";
                    cout << "Masukkan ulang: ";
                    cin >> mhs.jumlahHadir;
                }

                mhs.persentase = (mhs.jumlahHadir / 16.0f) * 100;
                mhs.isLulus = (mhs.persentase >= 75);

                daftarMahasiswa.push_back(mhs);
                simpanSemuaData(daftarMahasiswa);

                cout << "[INFO] Data berhasil disimpan ke rekap.csv.\n\n";

            } else if (pilihan == 2) {
                // LIHAT DATA
                cout << "\n=== REKAP PRESENSI MAHASISWA ===\n";
                if (daftarMahasiswa.size() == 0) {
                    cout << "Belum ada data presensi yang diinput.\n\n";
                } else {
                    cout << left << setw(5) << "No"
                        << setw(20) << "Nama"
                        << setw(15) << "Jumlah Hadir"
                        << setw(15) << "Persentase"
                        << setw(15) << "Lulus" << endl;
                    cout << "------------------------------------------------------\n";

                    for (size_t i = 0; i < daftarMahasiswa.size(); i++) {
                        Mahasiswa m = daftarMahasiswa[i];
                        cout << left << setw(5) << i+1
                            << setw(20) << m.nama
                            << setw(15) << m.jumlahHadir
                            << setw(15) << fixed << setprecision(2) << m.persentase
                            << (m.isLulus ? "Ya" : "Tidak") << endl;
                    }
                    cout << endl;
                }

            } else if (role == "admin" && pilihan == 3) {
                // EDIT DATA
                cout << "\n=== Edit Data Mahasiswa ===\n";
                if (daftarMahasiswa.empty()) {
                    cout << "Belum ada data untuk diedit.\n\n";
                } else {
                    for (size_t i = 0; i < daftarMahasiswa.size(); i++) {
                        cout << i+1 << ". " << daftarMahasiswa[i].nama << endl;
                    }
                    int idx;
                    cout << "Pilih nomor mahasiswa yang ingin diedit: ";
                    cin >> idx;
                    cin.ignore();
                    if (idx < 1 || idx > daftarMahasiswa.size()) {
                        cout << "[ERROR] Nomor tidak valid.\n\n";
                    } else {
                        Mahasiswa &mhs = daftarMahasiswa[idx-1];
                        cout << "Edit Nama (sebelumnya: " << mhs.nama << "): ";
                        getline(cin, mhs.nama);

                        cout << "Edit Jumlah Hadir (0-16, sebelumnya: " << mhs.jumlahHadir << "): ";
                        cin >> mhs.jumlahHadir;
                        while (mhs.jumlahHadir < 0 || mhs.jumlahHadir > 16) {
                            cout << "[ERROR] Jumlah hadir hanya boleh 0-16.\n";
                            cout << "Masukkan ulang: ";
                            cin >> mhs.jumlahHadir;
                        }

                        mhs.persentase = (mhs.jumlahHadir / 16.0f) * 100;
                        mhs.isLulus = (mhs.persentase >= 75);

                        simpanSemuaData(daftarMahasiswa);

                        cout << "[INFO] Data berhasil diperbarui!\n\n";
                    }
                }

            } else if (role == "admin" && pilihan == 4) {
                // HAPUS DATA
                cout << "\n=== Hapus Data Mahasiswa ===\n";
                if (daftarMahasiswa.empty()) {
                    cout << "Belum ada data untuk dihapus.\n\n";
                } else {
                    for (size_t i = 0; i < daftarMahasiswa.size(); i++) {
                        cout << i+1 << ". " << daftarMahasiswa[i].nama << endl;
                    }
                    int idx;
                    cout << "Pilih nomor mahasiswa yang ingin dihapus: ";
                    cin >> idx;
                    if (idx < 1 || idx > daftarMahasiswa.size()) {
                        cout << "[ERROR] Nomor tidak valid.\n\n";
                    } else {
                        cout << "[INFO] Data " << daftarMahasiswa[idx-1].nama << " berhasil dihapus.\n\n";
                        daftarMahasiswa.erase(daftarMahasiswa.begin() + (idx-1));
                        simpanSemuaData(daftarMahasiswa);
                    }
                }

            } else if ((role == "admin" && pilihan == 5) || (role == "user" && pilihan == 3)) {
                // LOGOUT
                cout << "\nAnda telah logout. Kembali ke halaman login...\n\n";
                logout = true;
                break;
            } else if ((role == "admin" && pilihan == 6) || (role == "user" && pilihan == 4)) {
                // KELUAR PROGRAM
                cout << "\nTerima kasih! Program selesai.\n";
                return 0;
            } else {
                cout << "[ERROR] Pilihan menu tidak valid.\n\n";
            }

        } while (!logout);
    }

    return 0;
}