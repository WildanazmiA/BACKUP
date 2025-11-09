#include <iostream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

struct Aktivitas {
    string nama;
    string kategori;
    int durasiMenit;
    bool selesai;
};

void tunggu(const string& pesan) {
    cout << "\n>>> " << pesan << ". Tekan ENTER untuk lanjut...\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
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
        cout << "7. Keluar\n";
        cout << "Pilih (1-7): ";
        getline(cin, pilihanStr);

        if (pilihanStr.empty()) {
            pilihan = 0;
        } else {
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
            cout << "Tugas '" << tugas.nama << "' ditambahkan.\n";
        }
        else if (pilihan == 2) {
            if (todoList.empty()) {
                cout << "To-Do List kosong.\n";
            } else {
                cout << "\n--- DAFTAR TUGAS ---\n";
                for (int i = 0; i < todoList.size(); i++) {
                    cout << i + 1 << ". [" << todoList[i].kategori << "] " << todoList[i].nama << "\n";
                }
            }
        }
        else if (pilihan == 3) {
            if (todoList.empty()) {
                cout << "Belum ada tugas.\n";
            } else {
                int nomor;
                cout << "\n--- PILIH TUGAS ---\n";
                for (int i = 0; i < todoList.size(); i++) {
                    cout << i + 1 << ". " << todoList[i].nama << " (" << todoList[i].kategori << ")\n";
                }
                cout << "Masukkan nomor tugas: ";
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
                    cout << "\n--- POMODORO ---\n";
                    cout << "Kerja 25 menit untuk tugas: " << todoList[nomor - 1].nama << "\n";
                    tunggu("Waktu kerja selesai");
                    cout << "Istirahat 5 menit.\n";
                    tunggu("Waktu istirahat selesai");

                    Aktivitas selesai = todoList[nomor - 1];
                    selesai.selesai = true;
                    selesai.durasiMenit = 25;
                    riwayat.push_back(selesai);
                    todoList.erase(todoList.begin() + nomor - 1);
                    cout << "Tugas selesai dan dicatat.\n";
                }
            }
        }
        else if (pilihan == 4) {
            int totalTugas = riwayat.size();
            int totalMenit = 0;
            for (int i = 0; i < riwayat.size(); i++) {
                totalMenit += riwayat[i].durasiMenit;
            }
            cout << "\n--- DASHBOARD ---\n";
            cout << "Total tugas selesai: " << totalTugas << "\n";
            cout << "Total waktu produktif: " << totalMenit << " menit (" << totalMenit / 60.0 << " jam)\n";
        }
        else if (pilihan == 5) {
            if (riwayat.empty()) {
                cout << "Belum ada aktivitas.\n";
            } else {
                cout << "\n--- RIWAYAT AKTIVITAS ---\n";
                for (int i = 0; i < riwayat.size(); i++) {
                    cout << i + 1 << ". [" << riwayat[i].kategori << "] "
                         << riwayat[i].nama << " | " << riwayat[i].durasiMenit << " menit\n";
                }
            }
        }
        else if (pilihan == 6) {
            if (!todoList.empty()) {
                cout << "Saran: Selesaikan tugas '" << todoList[0].nama << "'.\n";
            } else if (riwayat.empty()) {
                cout << "Saran: Tambahkan tugas baru dan mulai Pomodoro 25/5 menit.\n";
            } else {
                string kategoriKurang = riwayat[0].kategori;
                int waktuTerpendek = riwayat[0].durasiMenit;

                for (int i = 1; i < riwayat.size(); i++) {
                    if (riwayat[i].durasiMenit < waktuTerpendek) {
                        waktuTerpendek = riwayat[i].durasiMenit;
                        kategoriKurang = riwayat[i].kategori;
                    }
                }
                cout << "Saran: Tambah waktu untuk kategori '" << kategoriKurang << "'.\n";
            }
        }
        else if (pilihan == 7) {
            cout << "Program selesai.\n";
        }
        else {
            cout << "Pilihan tidak valid.\n";
        }

    } while (pilihan != 7);

    return 0;
}
