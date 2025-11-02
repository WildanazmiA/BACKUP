#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

struct Mahasiswa {
    string nama;
    int jumlahHadir;
    float persentaseHadir;
    bool isLulusKehadiran;
};

int main() {
    vector<Mahasiswa> daftarMahasiswa;
    int jumlahMahasiswa;

    cout << "Masukkan jumlah mahasiswa: ";
    cin >> jumlahMahasiswa;

    for (int i = 0; i < jumlahMahasiswa; i++) {
        Mahasiswa mhs;
        cout << "\nMasukkan nama mahasiswa ke-" << i + 1 << ": ";
        cin >> mhs.nama;
        cout << "Masukkan jumlah hadir (0-16): ";
        cin >> mhs.jumlahHadir;

        // Hitung persentase
        mhs.persentaseHadir = (mhs.jumlahHadir / 16.0f) * 100;
        mhs.isLulusKehadiran = (mhs.persentaseHadir >= 75);

        // Simpan ke vector
        daftarMahasiswa.push_back(mhs);
    }

    // Tampilkan hasil
    cout << "\n=== REKAP KEHADIRAN SEMUA MAHASISWA ===\n";
    cout << "-------------------------------------------------------------\n";
    cout << left << setw(8) << "No"
         << setw(15) << "Nama"
         << setw(10) << "Hadir"
         << setw(15) << "Persentase"
         << "Lulus" << endl;
    cout << "-------------------------------------------------------------\n";

    for (int i = 0; i < daftarMahasiswa.size(); i++) {
        cout << left << setw(8) << i + 1
             << setw(15) << daftarMahasiswa[i].nama
             << daftarMahasiswa[i].jumlahHadir << "/16" << setw(5) << " "
             << setw(10) << fixed << setprecision(2) << daftarMahasiswa[i].persentaseHadir << "%"
             << (daftarMahasiswa[i].isLulusKehadiran ? "Ya" : "Tidak") << endl;
    }

    cout << "-------------------------------------------------------------\n";
    cout << "Program selesai. Terima kasih!\n";

    return 0;
}