#include <iostream>
#include <fstream>
#include <cstdint>
#include "utils.h"

using namespace std;

void waitForKey() {
    cout << "Нажмите Enter для продолжения...";
    cin.get();
}

vector<unsigned char> readFile(const char* filename) {
    ifstream file(filename, ios::binary);
    return vector<unsigned char>((istreambuf_iterator<char>(file)), {});
}

void writeFile(const char* filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    file.write((const char*)data.data(), data.size());
}

void readFile(const char* filename, vector<mpz_class>& data) {
    ifstream file(filename, ios::binary);
    while (file.peek() != EOF) {
        uint32_t len = 0;
        file.read((char*)&len, sizeof(len));

        vector<unsigned char> buffer(len);
        file.read((char*)buffer.data(), len);

        mpz_class x;
        mpz_import(x.get_mpz_t(), len, 1, 1, 1, 0, buffer.data());
        data.push_back(x);
    }
}


void writeFile(const char* filename, const vector<mpz_class>& data) {
    ofstream file(filename, ios::binary);
    for (const auto& x : data) {
        size_t count = 0;
        vector<unsigned char> buffer((mpz_sizeinbase(x.get_mpz_t(), 2) + 7) / 8);
        mpz_export(buffer.data(), &count, 1, 1, 1, 0, x.get_mpz_t());

        uint32_t len = count;
        file.write((char*)&len, sizeof(len));
        file.write((char*)buffer.data(), len);
    }
}


void writeKey(const char* filename, const mpz_class& p, const mpz_class& g, const mpz_class& x) {
    ofstream file(filename);
    file << p << "\n" << g << "\n" << x << "\n";
}

void readKey(const char* filename, mpz_class& p, mpz_class& g, mpz_class& x) {
    ifstream file(filename);
    file >> p >> g >> x;
}
