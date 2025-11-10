#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;

struct Aktivitas {
    string nama;
    string kategori;
    int durasiMenit;
    bool selesai;
};

// Simpan data ke CSV
void simpanCSV(const vector<Aktivitas>& todo, const vector<Aktivitas>& riwayat) {
    ofstream file("data_aktivitas.csv");
    if (!file.is_open()) {
        cout << "Gagal menyimpan data ke CSV.\n";
        return;
    }

    file << "Nama,Kategori,DurasiMenit,Selesai\n";
    for (int i = 0; i < todo.size(); i++) {
        file << todo[i].nama << "," << todo[i].kategori << ","
            << todo[i].durasiMenit << "," << (todo[i].selesai ? "Ya" : "Tidak") << "\n";
    }
    for (int i = 0; i < riwayat.size(); i++) {
        file << riwayat[i].nama << "," << riwayat[i].kategori << ","
            << riwayat[i].durasiMenit << "," << (riwayat[i].selesai ? "Ya" : "Tidak") << "\n";
    }

    file.close();
}

// Tampilkan isi CSV
void tampilCSV() {
    ifstream file("data_aktivitas.csv");
    if (!file.is_open()) {
        cout << "File data_aktivitas.csv tidak bisa dibuka.\n";
        return;
    }

    string baris;
    cout << "\n--- DATA DARI CSV ---\n";
    while (getline(file, baris)) {
        stringstream ss(baris);
        string nama, kategori, durasi, selesai;

        getline(ss, nama, ',');
        getline(ss, kategori, ',');
        getline(ss, durasi, ',');
        getline(ss, selesai, ',');

        if (nama == "Nama") continue; // lewati header
        cout << "Nama: " << nama
            << ", Kategori: " << kategori
            << ", Durasi: " << durasi
            << ", Selesai: " << selesai << "\n";
    }

    file.close();
}

int main() {
    vector<Aktivitas> todoList;
    vector<Aktivitas> riwayat;
    int pilihan = 0;
    string pilihanStr;

    do {
        cout << "\n==============================\n";
        cout << "     PRODUCTIVITY TRACKER\n";
        cout << "==============================\n";
        cout << "1. Tambah Tugas\n";
        cout << "2. Lihat To-Do List\n";
        cout << "3. Mulai Pomodoro (25/5)\n";
        cout << "4. Dashboard Produktivitas\n";
        cout << "5. Lihat Track Record\n";
        cout << "6. Dapatkan Saran\n";
        cout << "7. Lihat Data CSV\n";
        cout << "8. Keluar\n";
        cout << "Pilih (1-8): ";
        getline(cin, pilihanStr);

        if (pilihanStr.empty()) pilihan = 0;
        else {
            try { pilihan = stoi(pilihanStr); }
            catch (...) { pilihan = 0; }
        }

        if (pilihan == 1) {
            Aktivitas tugas;
            cout << "Nama Tugas: ";
            getline(cin, tugas.nama);
            cout << "Kategori: ";
            getline(cin, tugas.kategori);
            tugas.durasiMenit = 0;
            tugas.selesai = false;
            todoList.push_back(tugas);
            simpanCSV(todoList, riwayat);
            cout << "Tugas disimpan ke CSV.\n";
        } 
        else {
            if (pilihan == 2) {
                if (todoList.empty()) {
                    cout << "To-Do List kosong.\n";
                } else {
                    cout << "\n--- DAFTAR TUGAS ---\n";
                    for (int i = 0; i < todoList.size(); i++) {
                        cout << i + 1 << ". [" << todoList[i].kategori << "] " << todoList[i].nama << "\n";
                    }
                }
            } 
            else {
                if (pilihan == 3) {
                    if (todoList.empty()) {
                        cout << "Belum ada tugas.\n";
                    } else {
                        int nomor;
                        cout << "\nPilih tugas yang ingin dikerjakan:\n";
                        for (int i = 0; i < todoList.size(); i++) {
                            cout << i + 1 << ". " << todoList[i].nama << " (" << todoList[i].kategori << ")\n";
                        }
                        cout << "Nomor: ";
                        if (!(cin >> nomor)) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Input tidak valid.\n";
                            continue;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        if (nomor < 1 || nomor > todoList.size()) {
                            cout << "Nomor tidak valid.\n";
                        } else {
                            cout << "\nKerja 25 menit untuk tugas: " << todoList[nomor - 1].nama << "\n";
                            cout << "Istirahat 5 menit.\n";

                            Aktivitas selesai = todoList[nomor - 1];
                            selesai.selesai = true;
                            selesai.durasiMenit = 25;
                            riwayat.push_back(selesai);
                            todoList.erase(todoList.begin() + nomor - 1);
                            simpanCSV(todoList, riwayat);
                            cout << "Tugas selesai dan disimpan ke CSV.\n";
                        }
                    }
                } 
                else {
                    if (pilihan == 4) {
                        int totalTugas = riwayat.size();
                        int totalMenit = 0;
                        for (int i = 0; i < riwayat.size(); i++) totalMenit += riwayat[i].durasiMenit;
                        cout << "\n--- DASHBOARD ---\n";
                        cout << "Total tugas selesai: " << totalTugas << "\n";
                        cout << "Total waktu produktif: " << totalMenit << " menit (" << totalMenit / 60.0 << " jam)\n";
                    } 
                    else {
                        if (pilihan == 5) {
                            if (riwayat.empty()) cout << "Belum ada aktivitas.\n";
                            else {
                                cout << "\n--- RIWAYAT ---\n";
                                for (int i = 0; i < riwayat.size(); i++) {
                                    cout << i + 1 << ". [" << riwayat[i].kategori << "] "
                                        << riwayat[i].nama << " | " << riwayat[i].durasiMenit << " menit\n";
                                }
                            }
                        } 
                        else {
                            if (pilihan == 6) {
                                if (!todoList.empty()) cout << "Saran: Selesaikan tugas '" << todoList[0].nama << "'.\n";
                                else if (riwayat.empty()) cout << "Saran: Tambahkan tugas baru.\n";
                                else cout << "Saran: Tambah durasi untuk kategori yang paling sedikit dikerjakan.\n";
                            } 
                            else if (pilihan == 7) {
                                tampilCSV();
                            } 
                            else if (pilihan == 8) {
                                cout << "Program selesai.\n";
                            } 
                            else {
                                cout << "Pilihan tidak valid.\n";
                            }
                        }
                    }
                }
            }
        }
    } while (pilihan != 8);

    return 0;
}
