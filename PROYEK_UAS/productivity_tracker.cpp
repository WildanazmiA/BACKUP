#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <thread>
#include <map> // Memastikan map di-include
#include <limits> // Untuk numeric_limits

using namespace std;
using namespace std::chrono;

// --- STRUKTUR DATA ---
struct Aktivitas {
    string nama;
    string kategori;
    long durasiMenit = 0;
    bool selesai = false;
};

// --- KELAS UTAMA: TRACKER ---
class ProductivityTracker {
private:
    vector<Aktivitas> todoList;
    vector<Aktivitas> riwayatAktivitas;

    string getCurrentTime() const {
        time_t now = system_clock::to_time_t(system_clock::now());
        stringstream ss;
        ss << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    void simpanDanTunggu(const string& pesan) const {
        cout << "\n>>> " << pesan << ". Tekan ENTER untuk melanjutkan...\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

public:
    // SEMUA FUNGSI HARUS DI BAGIAN PUBLIC AGAR DAPAT DIPANGGIL DARI MAIN()
    ProductivityTracker() {}
    
    void tugasAwal(const string& nama, const string& kategori) {
        todoList.push_back({nama, kategori, 0, false});
    }

    // 1. To-Do List
    void tambahTugas(const string& nama, const string& kategori) {
        todoList.push_back({nama, kategori, 0, false});
        cout << " Tugas '" << nama << "' kategori '" << kategori << "' ditambahkan.\n";
    }

    void tandaiSelesai(int indeks, long durasi) {
        if (indeks >= 1 && indeks <= todoList.size()) {
            Aktivitas& tugas = todoList[indeks - 1];
            tugas.selesai = true;
            tugas.durasiMenit = durasi;
            riwayatAktivitas.push_back(tugas);
            todoList.erase(todoList.begin() + indeks - 1);
            cout << " Tugas " << indeks << " selesai dan dicatat. Durasi: " << durasi << " menit.\n";
        } else {
            cout << " Indeks tugas tidak valid.\n";
        }
    }

    void tampilkanTodoList() const {
        if (todoList.empty()) {
            cout << " To-Do List kosong. Tambahkan tugas baru.\n";
            return;
        }
        cout << "\n--- DAFTAR TUGAS (TO-DO LIST) ---\n";
        for (size_t i = 0; i < todoList.size(); ++i) {
            cout << i + 1 << ". [" << todoList[i].kategori << "] " << todoList[i].nama << "\n";
        }
        cout << "----------------------------------\n";
    }

    // 2. Pomodoro Timer
    void mulaiPomodoro(int kerjaMenit, int istirahatMenit) {
        int indeksTugas;
        tampilkanTodoList();
        if (todoList.empty()) return;
        
        cout << "Pilih nomor tugas yang akan dikerjakan: ";
        // Menggunakan cin >> int untuk input tunggal di sini lebih efisien
        if (!(cin >> indeksTugas)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        
        if (indeksTugas < 1 || indeksTugas > todoList.size()) {
            cout << " Pilihan tidak valid.\n";
            return;
        }
        long durasiTotal = 0;
        
        cout << "\n--- POMODORO DIMULAI ---\n";
        cout << " Sesi KERJA: " << kerjaMenit << " menit. Tugas: " << todoList[indeksTugas - 1].nama << "\n";
        simpanDanTunggu("Waktu kerja selesai");
        durasiTotal += kerjaMenit;
        cout << "Waktu kerja dicatat: " << kerjaMenit << " menit.\n";
        cout << "\n Sesi ISTIRAHAT: " << istirahatMenit << " menit.\n";
        simpanDanTunggu("Waktu istirahat selesai");
        tandaiSelesai(indeksTugas, durasiTotal);
    }

    // 3. Track Record Aktivitas (dipanggil di case 5)
    void tampilkanTrackRecord() const {
        if (riwayatAktivitas.empty()) {
            cout << " Track Record kosong. Mulai kerjakan tugas!\n";
            return;
        }
        cout << "\n--- RIWAYAT AKTIVITAS LENGKAP ---\n";
        for (size_t i = 0; i < riwayatAktivitas.size(); ++i) {
            cout << i + 1 << ". [" << riwayatAktivitas[i].kategori << "] " 
                 << riwayatAktivitas[i].nama << " | Durasi: " 
                 << riwayatAktivitas[i].durasiMenit << " menit\n";
        }
        cout << "--------------------------------\n";
    }

    // 4. Dashboard Produktivitas (dipanggil di case 4)
    void tampilkanDashboard() const {
        long totalWaktu = 0;
        for (const auto& a : riwayatAktivitas) {
            totalWaktu += a.durasiMenit;
        }

        cout << "\n--- DASHBOARD PRODUKTIVITAS ---\n";
        cout << "Total Tugas Selesai: " << riwayatAktivitas.size() << ".\n";
        cout << "Total Waktu Produktif: " << totalWaktu << " menit (" << totalWaktu / 60.0 << " jam).\n";
        
        // Memastikan map berfungsi dengan benar
        map<string, long> waktuPerKategori;
        for (const auto& a : riwayatAktivitas) {
            waktuPerKategori[a.kategori] += a.durasiMenit;
        }

        cout << "\n**Waktu Per Kategori:**\n";
        for (const auto& pair : waktuPerKategori) {
            cout << "- " << pair.first << ": " << pair.second << " menit.\n";
        }
        cout << "---------------------------------\n";
    }

    // 5. Saran Kegiatan (dipanggil di case 6)
    string berikanSaranKegiatan() const {
        if (!todoList.empty()) {
            return " Fokus selesaikan tugas pertama di To-Do List: '" + todoList[0].nama + "'.";
        }
        if (riwayatAktivitas.empty()) {
            return " Tambahkan tugas dan coba sesi Pomodoro 25/5 menit untuk memulai. Anda perlu membangun momentum.";
        }
        
        map<string, long> waktuPerKategori;
        for (const auto& a : riwayatAktivitas) {
            waktuPerKategori[a.kategori] += a.durasiMenit;
        }

        if (waktuPerKategori.empty()) {
            return " Tambahkan tugas baru ke To-Do List Anda.";
        }

        auto saran = min_element(waktuPerKategori.begin(), waktuPerKategori.end(),
                                [](const auto& a, const auto& b) {
                                    return a.second < b.second;
                                });

        return " Coba alokasikan waktu untuk kategori yang kurang terekam: '" + saran->first + "'.";
    }
};

// --- FUNGSI UTAMA (MAIN) ---
int main() {;
    ProductivityTracker tracker;
    int pilihan = 0; 
    string pilihanStr;

    do {
        cout << "\n===================================\n";
        cout << "     PRODUCTIVITY TRACKER\n";
        cout << "===================================\n";
        cout << "1. Tambah Tugas\n";
        cout << "2. Lihat To-Do List\n";
        cout << "3. Mulai Pomodoro Timer (25/5)\n";
        cout << "4. Tampilkan Dashboard Produktivitas\n";
        cout << "5. Tampilkan Track Record Aktivitas\n";
        cout << "6. Dapatkan Saran Kegiatan\n";
        cout << "7. Keluar\n";
        cout << "Pilih fitur (1-7): " << endl; // Menggunakan endl untuk flush output

        
        // Membaca input menu menggunakan getline (PALING STABIL)
        if (!getline(cin, pilihanStr) || pilihanStr.empty()) {
            pilihan = 0;
            continue;
        }
        
        // Konversi string ke integer
        try {
            pilihan = stoi(pilihanStr);
        } catch (...) {
            pilihan = 0;
            cout << "Pilihan harus berupa angka.\n";
            continue;
        }

        switch (pilihan) {
            case 1: {
                string nama, kategori;
                
                // TIDAK PERLU CIN.IGNORE ATAU FLUSH KARENA INPUT SEBELUMNYA (pilihan) ADALAH GETLINE
                cout << "Masukkan Nama Tugas: "; 
                cout.flush(); // Tetap gunakan flush untuk keamanan maksimal
                getline(cin, nama); // Input nama tugas
                
                cout << "Masukkan Kategori (Kerja, Edukasi, Rumah, dll.): ";
                cout.flush();
                getline(cin, kategori); // Input kategori
                
                tracker.tambahTugas(nama, kategori);
                break;
            }
            case 2:
                tracker.tampilkanTodoList();
                break;
            case 3:
                // Case 3 tetap membutuhkan penanganan buffer internal karena ada cin >> int
                tracker.mulaiPomodoro(25, 5);
                break;
            case 4:
                tracker.tampilkanDashboard();
                break;
            case 5:
                tracker.tampilkanTrackRecord();
                break;
            case 6:
                cout << "\n=== SARAN ===\n";
                cout << tracker.berikanSaranKegiatan() << "\n";
                break;
            case 7:
                cout << "Program selesai. Data tidak disimpan permanen.\n";
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (pilihan != 7);

    return 0;
}