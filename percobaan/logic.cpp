#include <iostream>
#include <string>

using namespace std;

int main() {
    string input;
    cout << "Masukkan sesuatu: ";
    getline(cin, input);
    cout << "Anda memasukkan: " << input << endl;
    cin.get();
    return 0;
}
