#include <iostream>
#include <bitset>
#include <string>
using namespace std;

// Fungsi untuk menampilkan konversi setiap huruf ke biner
void konversiNamaKeBitwise(const string &nama) {
    cout << "\n=== KONVERSI NAMA KE BINARY (BITWISE) ===" << endl;
    for (size_t i = 0; i < nama.length(); i++) {
        char c = nama[i];
        int ascii = int(c);
        cout << "'" << c << "' = ASCII " << ascii
        << " = " << bitset<8>(ascii) << endl;
    }
}

// Fungsi untuk operasi XOR antar huruf
void operasiXORNama(const string &nama) {
    if (nama.length() < 2) {
        cout << "Minimal 2 huruf untuk operasi XOR.\n";
        return;
    }

    cout << "\n=== OPERASI XOR ANTAR KARAKTER ===" << endl;
    for (size_t i = 0; i < nama.length() - 1; i++) {
        unsigned char a = nama[i];
        unsigned char b = nama[i + 1];
        unsigned char hasil = a ^ b;

        cout << "'" << a << "' (" << bitset<8>(a) << ") XOR '"
            << b << "' (" << bitset<8>(b) << ") = "
            << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
    }
}

// Fungsi untuk operasi NOT pada setiap huruf
void operasiNOTNama(const string &nama) {
    cout << "\n=== OPERASI NOT PADA KARAKTER ===" << endl;
    for (size_t i = 0; i < nama.length(); i++) {
        unsigned char a = nama[i];
        unsigned char hasil = ~a;

        cout << "NOT '" << a << "' (" << bitset<8>(a) << ") = "
            << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
    }
}

// Fungsi untuk operasi AND antar huruf
void operasiANDNama(const string &nama) {
    if (nama.length() < 2) {
        cout << "Minimal 2 huruf untuk operasi AND.\n";
        return;
    }

    cout << "\n=== OPERASI AND ANTAR KARAKTER ===" << endl;
    for (size_t i = 0; i < nama.length() - 1; i++) {
        unsigned char a = nama[i];
        unsigned char b = nama[i + 1];
        unsigned char hasil = a & b;

        cout << "'" << a << "' (" << bitset<8>(a) << ") AND '"
            << b << "' (" << bitset<8>(b) << ") = "
            << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
    }
}


// Fungsi untuk operasi OR antar huruf
void operasiORNama(const string &nama) {
    if (nama.length() < 2) {
        cout << "Minimal 2 huruf untuk operasi OR.\n";
        return;
    }

    cout << "\n=== OPERASI OR ANTAR KARAKTER ===" << endl;
    for (size_t i = 0; i < nama.length() - 1; i++) {
        unsigned char a = nama[i];
        unsigned char b = nama[i + 1];
        unsigned char hasil = a | b;

        cout << "'" << a << "' (" << bitset<8>(a) << ") OR '"
            << b << "' (" << bitset<8>(b) << ") = "
            << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
    }
}


// Menu utama
void tampilkanMenu() {
    cout << "\n=== MENU KALKULATOR BITWISE NAMA ===" << endl;
    cout << "1. Konversi huruf ke biner" << endl;
    cout << "2. Operasi XOR antar huruf" << endl;
    cout << "3. Operasi NOT antar huruf" << endl;
    cout << "4. Operasi AND antar huruf" << endl;
    cout << "5. Operasi OR antar huruf" << endl;    
    cout << "6. Ganti nama" << endl;
    cout << "7. Keluar" << endl;
    cout << "====================================" << endl;
}

int main() {
    string nama;
    int pilihan;
    bool jalan = true;

    cout << "=== KALKULATOR NAMA KE BITWISE ===" << endl;
    cout << "Masukkan nama Anda: ";
    getline(cin, nama);

    while (jalan) {
        tampilkanMenu();
        cout << "Pilih menu (1-4): ";
        cin >> pilihan;

        switch (pilihan) {
            case 1:
                konversiNamaKeBitwise(nama);
                break;

            case 2:
                operasiXORNama(nama);
                break;

            case 3:
                operasiNOTNama(nama);
                break;
            
            case 4:
                operasiANDNama(nama);
                break;

            case 5:
                operasiORNama(nama);
                break;

            case 6:
                cout << "\nMasukkan nama baru: ";
                cin.ignore(); // hapus newline dari input sebelumnya
                getline(cin, nama);
                break;

            case 7:
                cout << "\nTerima kasih telah menggunakan kalkulator bitwise!" << endl;
                jalan = false;
                cin.get();
                break;

            default:
                cout << "Pilihan tidak valid! Coba lagi.\n";
        }
    }

    return 0;
}