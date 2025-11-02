#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    const int n = 3;
    double A[n][n+1] = {
        {-2, 4, -5, -13}
        {4, -3, -16}
        {1, -2, 2, 6}
    };

    cout << fixed << setprecision(2);
    cout << "Matriks awal:\n";
    for (int i=0; i<n; i++) {
        for (int j=0; j<=n; j++) cout << setw(8) << A[i][j];
        cout << '\n';
    }

    // Gauss-Jordan penuh: untuk setiap baris i
    for (int i = 0; i < n; i++) {
        // Jika diagonal A[i][i] = 0, coba tukar dengan baris di bawahnya yang tidak nol
        if (A[i][i] == 0) {
            int swap_row = -1;
            for (int r = i+1; r < n; r++) {
                if (A[r][i] != 0) { swap_row = r; break; }
            }
            if (swap_row != -1) {
                cout << "\nOperasi: Tukar R" << i+1 << " <-> R" << swap_row+1 << '\n';
                for (int c = 0; c <= n; c++) swap(A[i][c], A[swap_row][c]);
                // cetak matriks
                for (int r = 0; r < n; r++) {
                    for (int c = 0; c <= n; c++) cout << setw(8) << A[r][c];
                    cout << '\n';
                }
            } else {
                cout << "\nTidak bisa lanjut, diagonal nol dan tidak ada baris untuk ditukar.\n";
                cout << "Tekan Enter untuk keluar...";
                cin.ignore(); cin.get();
                return 0;
            }
        }

        // Normalisasi: buat A[i][i] = 1 dengan membagi seluruh baris
        double diag = A[i][i];
        cout << "\nOperasi: R" << i+1 << " = R" << i+1 << " / (" << diag << ")\n";
        for (int c = 0; c <= n; c++) A[i][c] /= diag;
        // cetak matriks
        for (int r = 0; r < n; r++) {
            for (int c = 0; c <= n; c++) cout << setw(8) << A[r][c];
            cout << '\n';
        }

        // Eliminasi: buat semua baris k != i di kolom i menjadi 0
        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            double factor = A[k][i];
            if (factor == 0) continue;
            cout << "\nOperasi: R" << k+1 << " = R" << k+1 << " - (" << factor << ")*R" << i+1 << '\n';
            for (int c = 0; c <= n; c++) {
                A[k][c] -= factor * A[i][c];
            }
            // cetak matriks setelah tiap operasi baris
            for (int r = 0; r < n; r++) {
                for (int c = 0; c <= n; c++) cout << setw(8) << A[r][c];
                cout << '\n';
            }
        }
    }

    cout << "\nMatriks akhir (harus identitas):\n";
    for (int i=0; i<n; i++) {
        for (int j=0; j<=n; j++) cout << setw(8) << A[i][j];
        cout << '\n';
    }

    cout << "\nTekan Enter untuk keluar...";
    cin.ignore(); cin.get();
    return 0;
}