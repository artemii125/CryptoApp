#include <vector>
#include <random>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "utils.h"

using namespace std;

//генерация ключа перестановки
vector<int> generatePermutationKey(int blockSize) {
    vector<int> key(blockSize); //создание ключа
    for (int i = 0; i < blockSize; ++i) key[i] = i;
    random_device rd;
    mt19937 gen(rd()); //генерация случайных чисел
    shuffle(key.begin(), key.end(), gen); //перемешивание - перестановка
    return key;
}

//применение перестановки
void applyPermutation(vector<unsigned char>& block, const vector<int>& key) {
    vector<unsigned char> temp = block; //копия блока
    for (size_t i = 0; i < key.size(); ++i)
        block[i] = temp[key[i]]; //меняем порядок
}

//обратная перестановка
vector<int> inverseKey(const vector<int>& key) {
    vector<int> inv(key.size());
    for (size_t i = 0; i < key.size(); ++i)
        inv[key[i]] = i;
    return inv; 
}

extern "C" {

//шифрование файла
void permutationEncryptFile(const char* inputFile, const char* outputFile, const char* keyFile) {
    auto data = readFile(inputFile); //чтение файла
    int blockSize = 8; //заданный стандартный размер блока
    auto key = generatePermutationKey(blockSize); //генерация ключа

    while (data.size() % blockSize != 0) //дополнение до кратности блока
        data.push_back(0);

    for (size_t i = 0; i < data.size(); i += blockSize) {
        vector<unsigned char> block(data.begin() + i, data.begin() + i + blockSize);
        applyPermutation(block, key); //применение перестановки
        copy(block.begin(), block.end(), data.begin() + i);
    }

    writeFile(outputFile, data); //запись зашфрованных данных
    vector<unsigned char> keyBytes(key.begin(), key.end());
    writeFile(keyFile, keyBytes); //запись ключа
}

//дешифрование файла
void permutationDecryptFile(const char* inputFile, const char* keyFile, const char* outputFile) {
    //загрузка необходмых данных
    auto data = readFile(inputFile);
    auto keyBytes = readFile(keyFile);
    vector<int> key(keyBytes.begin(), keyBytes.end());
    auto invKey = inverseKey(key);
    int blockSize = invKey.size();

    for (size_t i = 0; i < data.size(); i += blockSize) {
        vector<unsigned char> block(data.begin() + i, data.begin() + i + blockSize);
        applyPermutation(block, invKey); //применение обратной перестановки
        copy(block.begin(), block.end(), data.begin() + i); 
    }

    writeFile(outputFile, data); //запись расшифрованных данных
}

//шифрование текста
const char* permutationEncryptText(const char* text, const char* keyFile) {
    static string result;
    result.clear();

    vector<unsigned char> data(text, text + strlen(text)); //перевод текста в байты
    int blockSize = 8;
    auto key = generatePermutationKey(blockSize);

    while (data.size() % blockSize != 0) //дополнение нулями
        data.push_back(0);

    for (size_t i = 0; i < data.size(); i += blockSize) {
        vector<unsigned char> block(data.begin() + i, data.begin() + i + blockSize);
        applyPermutation(block, key);
        copy(block.begin(), block.end(), data.begin() + i);
    }

    //конвертация в hex
    stringstream ss;
    for (unsigned char c : data)
        ss << hex << setw(2) << setfill('0') << (int)c;

    result = ss.str();

    vector<unsigned char> keyBytes(key.begin(), key.end());
    writeFile(keyFile, keyBytes); //сохранение ключ

    return result.c_str(); //возврат hex строки
}

//дешифрование текста
const char* permutationDecryptText(const char* ciphertext, const char* keyFile) {
    static string result;
    result.clear();

    //hex в bytes
    vector<unsigned char> data;
    for (size_t i = 0; i < strlen(ciphertext); i += 2) {
        string byteStr(ciphertext + i, 2);
        data.push_back((unsigned char)stoul(byteStr, nullptr, 16));
    }

    auto keyBytes = readFile(keyFile); //загрузка ключа
    vector<int> key(keyBytes.begin(), keyBytes.end());
    auto invKey = inverseKey(key);
    int blockSize = invKey.size();

    for (size_t i = 0; i < data.size(); i += blockSize) {
        vector<unsigned char> block(data.begin() + i, data.begin() + i + blockSize);
        applyPermutation(block, invKey);
        copy(block.begin(), block.end(), data.begin() + i);
    }

    result.assign(data.begin(), data.end()); //сборка строки из байтов
    return result.c_str(); //возвращение строки
}

}
