#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Masukan jumlah persegi panjang yang ingin dihitung: ";
    cin >> n;

    int panjang, lebar, totalLuas = 0;

    for (int i = 1; i <= n; i++) {
        cout << "\nPersegi panjang ke-" << i << endl;
        cout << "Masukan panjang: ";
        cin >> panjang;
        cout << "Masukan lebar: ";
        cin >> lebar;

        int luas = panjang * lebar;
        cout << "Luas persegi panjang ke-" << i << " = " << luas << endl;

        totalLuas += luas;
    }

    cout << "\nTotal luas dari " << n << " persegi panjang adalah: " << totalLuas << endl;

    return 0;
}
