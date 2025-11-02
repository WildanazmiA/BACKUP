#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    int n;
    cout << "Masukkan jumlah persamaan: ";
    cin >> n;

    double A[10][11]; // Ukuran maksimum 10x10 agar fleksibel
    cout << "Masukkan augmented matrix (" << n << "x" << n+1 << "):\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            cin >> A[i][j];
        }
    }

    cout << "\nMatriks awal:\n";
    cout << fixed << setprecision(2);
    for (int i=0; i<n; i++) {
        for (int j=0; j<=n; j++) cout << setw(10) << A[i][j];
        cout << endl;
    }

    // Eliminasi baris otomatis tanpa pivot dan tanpa substitusi
    for (int i = 0; i < n; i++) {
        double pembagi = A[i][i];
        if (pembagi == 0) continue;
        cout << "\nLangkah: Bagi baris " << i+1 << " dengan " << pembagi << endl;
        for (int j = 0; j <= n; j++) A[i][j] /= pembagi;

        // Cetak hasil setelah pembagian
        for (int r = 0; r < n; r++) {
            for (int c = 0; c <= n; c++) cout << setw(10) << A[r][c];
            cout << endl;
        }

        // Hilangkan nilai di kolom i untuk baris lain
        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            double faktor = A[k][i];
            if (faktor == 0) continue;
            cout << "\nLangkah: R" << k+1 << " = R" << k+1 << " - (" << faktor << ")*R" << i+1 << endl;
            for (int j = 0; j <= n; j++) A[k][j] -= faktor * A[i][j];

            // Cetak hasil sementara
            for (int r = 0; r < n; r++) {
                for (int c = 0; c <= n; c++) cout << setw(10) << A[r][c];
                cout << endl;
            }
        }
    }

    cout << "\nMatriks akhir:\n";
    for (int i=0; i<n; i++) {
        for (int j=0; j<=n; j++) cout << setw(10) << A[i][j];
        cout << endl;
    }

    // Menampilkan hasil akhir variabel
    cout << "\nHasil akhir variabel:\n";
    for (int i=0; i<n; i++) {
        cout << "x" << i+1 << " = " << A[i][n] << endl;
    }

    return 0;
}
