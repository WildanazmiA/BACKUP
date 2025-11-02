#include <iostream>
#include <bitset>
#include <string>

using namespace std;

int main() {
    string nama;
    int pilihan;
    bool jalan = true;

    cout << "=== KALKULATOR NAMA KE BITWISE ===" << endl;
    cout << "Masukkan nama Anda: ";
    getline(cin, nama);

    while (jalan){
        cout << "\n=== MENU KALKULATOR BITWISE NAMA ===" << endl;
        cout << "1. Konversi huruf ke biner" << endl;
        cout << "2. Operasi XOR antar huruf" << endl;
        cout << "3. Operasi NOT antar huruf" << endl;
        cout << "4. Operasi AND antar huruf" << endl;
        cout << "5. Operasi OR antar huruf" << endl;    
        cout << "6. Ganti nama" << endl;
        cout << "7. Keluar" << endl;
        cout << "====================================" << endl;

        cout << "Pilih menu (1-7): ";
        cin >> pilihan;

        if  (pilihan == 1) {
            cout << "\n== Konversi huruf ke biner ==" << endl;\
            for (size_t i= 0; i < nama.length(); i++) {
                unsigned char huruf = nama[i];
                cout << "'" << huruf << "' (" << bitset<8>(huruf) << ") = ASCII " << int(huruf) << endl;
            }
        }

        else if (pilihan == 2) {
            if  (nama.length() < 2) {
                cout << "Operasi XOR membutuhkan setidaknya dua huruf." << endl;
            } else {
                cout << "\n== Operasi XOR antar huruf ==" << endl;
                for (size_t i = 0; i < nama.length() - 1; i++) {
                    unsigned char a = nama[i];
                    unsigned char b = nama[i + 1];
                    unsigned char hasil = a ^ b;

                    cout << "'" << a << "' (" << bitset<8>(a) << ") XOR '"
                        << b << "' (" << bitset<8>(b) << ") = "
                        << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
                }
            }
        }
        else if (pilihan == 3) {
            if (nama.length() < 1) {
                cout << "\n == Operasi NOT pada karakter ==" << endl;
                for (size_t i = 0; i < nama.length(); i++) {
                    unsigned char a = nama[i];
                    unsigned char hasil = ~a;
                    cout << "NOT '" << a << "' (" << bitset<8>(a) << ") = "
                        << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
                }
            }
        }
        else if (pilihan == 4) {
            if (nama.length() < 2) {
                cout << "Operasi AND membutuhkan setidaknya dua huruf." << endl;
            } else {
                cout << "\n== Operasi AND antar huruf ==" << endl;
                for (size_t i = 0; i < nama.length() - 1; i++) {
                    unsigned char a = nama[i];
                    unsigned char b = nama[i + 1];
                    unsigned char hasil = a & b;

                    cout << "'" << a << "' (" << bitset<8>(a) << ") AND '"
                        << b << "' (" << bitset<8>(b) << ") = "
                        << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
                }
            }
        }
        else if (pilihan == 5) {
            if (nama.length() < 2) {
                cout << "Operasi OR membutuhkan setidaknya dua huruf." << endl;
            } else {
                cout << "\n== Operasi OR antar huruf ==" << endl;
                for (size_t i = 0; i < nama.length() - 1; i++) {
                    unsigned char a = nama[i];
                    unsigned char b = nama[i + 1];
                    unsigned char hasil = a | b;

                    cout << "'" << a << "' (" << bitset<8>(a) << ") OR '"
                        << b << "' (" << bitset<8>(b) << ") = "
                        << bitset<8>(hasil) << " (ASCII " << int(hasil) << ")" << endl;
                }
            }
        }
        else if (pilihan == 6) {
            cin.ignore(); // Membersihkan newline dari input sebelumnya
            cout << "Masukkan nama baru Anda: ";
            getline(cin, nama);
        }
        else if (pilihan == 7) {
            cout << "Keluar dari program. Terima kasih!" << endl;
            jalan = false;
        }
        else {
            cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
    }
    return 0;
}