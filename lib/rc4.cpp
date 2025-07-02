#include <vector>
#include <random>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include "utils.h"
using namespace std;

//генерация ключа заданной длины
vector<unsigned char> generateKey(size_t length) {
    vector<unsigned char> key(length);
    random_device rd;
    for (auto& b : key) b = rd() % 256; //случацный байт от 0 до 255
    return key;
}

//иницализация массива RC4 (S-блок)
void rc4Init(unsigned char* s, const unsigned char* key, size_t keyLen) {
    for (int i = 0; i < 256; ++i) s[i] = i; //инициализация блока
    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + s[i] + key[i % keyLen]) % 256; //смесь ключа и блока
        swap(s[i], s[j]); //перрестановка
    }
}

//алгоритм RC4
void rc4Apply(unsigned char* s, unsigned char* data, size_t len) {
    int i = 0, j = 0;
    for (size_t k = 0; k < len; ++k) {
        i = (i + 1) % 256;
        j = (j + s[i]) % 256;
        swap(s[i], s[j]);
        unsigned char rnd = s[(s[i] + s[j]) % 256]; //генерация псевдослучайного байта
        data[k] ^= rnd; //применение XOR к данным
    }
}

extern "C" {

//шифрование файла
void rc4EncryptFile(const char* inputFile, const char* outputFile, const char* keyFile) {
    auto data = readFile(inputFile); 
    auto key = generateKey(16); //генерация 16 байтого ключа
    unsigned char s[256]; rc4Init(s, key.data(), key.size());
    rc4Apply(s, data.data(), data.size()); //шифрование 
    writeFile(outputFile, data); 
    writeFile(keyFile, key);
}

//расшфрование файла 
void rc4DecryptFile(const char* inputFile, const char* keyFile, const char* outputFile) {
    auto data = readFile(inputFile); 
    auto key = readFile(keyFile); 
    unsigned char s[256]; rc4Init(s, key.data(), key.size());
    rc4Apply(s, data.data(), data.size()); //расшифровка (XOR)
    writeFile(outputFile, data); //сохранение расшифрованных данных
}

//шифрование текста
const char* rc4EncryptText(const char* text, const char* keyFile) {
    vector<unsigned char> data(text, text + strlen(text)); //перевод текста в байты
    auto key = generateKey(16);
    unsigned char s[256]; rc4Init(s, key.data(), key.size());
    rc4Apply(s, data.data(), data.size());

    stringstream ss;
    for (auto c : data) 
        ss << hex << setw(2) << setfill('0') << (int)c; //перевод в hex строку


    writeFile(keyFile, key);
    string result = ss.str();

    char* out = new char[result.size() + 1];
    strcpy(out, result.c_str());
    return out;
}

//расшифрование текста
const char* rc4DecryptText(const char* ciphertext, const char* keyFile) {
    static string result;
    vector<unsigned char> data;
    for (size_t i = 0; i < strlen(ciphertext); i += 2) {
        string byteStr(ciphertext + i, 2); //каждые 2 символа = 1 байт
        data.push_back((unsigned char)stoul(byteStr, nullptr, 16));
    }
    auto key = readFile(keyFile);
    unsigned char s[256]; 
    rc4Init(s, key.data(), key.size());
    rc4Apply(s, data.data(), data.size());

    result.assign(data.begin(), data.end()); //расшифровка
    return result.c_str();
}

}
