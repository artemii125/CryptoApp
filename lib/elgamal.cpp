#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <gmpxx.h>
#include "utils.h"
using namespace std;

//поиск простого числа с заданной битностью
mpz_class generatePrime(int bits = 32) {
    gmp_randclass rng(gmp_randinit_mt);
    rng.seed(time(NULL));
    mpz_class p = rng.get_z_bits(bits);
    mpz_nextprime(p.get_mpz_t(), p.get_mpz_t());
    return p;
}

//проверка на то, что g первообразный корень по модулю p
bool isPrimitiveRoot(const mpz_class& g, const mpz_class& p) {
    mpz_class phi = p - 1; //функция Эйлера по фи
    for (mpz_class i = 2; i * i <= phi; ++i) {
        if (phi % i == 0) {
            mpz_class q;
            mpz_class exp = phi / i;
            //проверка на то что g^(phi/i)) mod p не равен 1
            if (mpz_powm_ui(q.get_mpz_t(), g.get_mpz_t(), exp.get_ui(), p.get_mpz_t()), q == 1) return false;
            if (mpz_powm_ui(q.get_mpz_t(), g.get_mpz_t(), i.get_ui(), p.get_mpz_t()), q == 1) return false;
        }
    }
    return true;
}


mpz_class findGenerator(const mpz_class& p) {
    for (mpz_class g = 2; g < p; ++g)
        if (isPrimitiveRoot(g, p)) return g; //возврат первого найденного корня
    throw runtime_error("Не найден первообразный корень по модулю p");
}

extern "C" {

//шифрование файла
void elgamalEncryptFile(const char* inputFile, const char* outputFile, const char* keyFile) {
    vector<unsigned char> text = readFile(inputFile); //чтение файла в вектор байт

    mpz_class p, g, x, y; 
    gmp_randclass rng(gmp_randinit_mt);
    rng.seed(time(NULL));

    //генерация параметров
    p = generatePrime(32);
    g = findGenerator(p);
    x = rng.get_z_range(p - 2) + 1;
    mpz_powm(y.get_mpz_t(), g.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t()); //y = g^x mod p

    vector<mpz_class> encrypted = {p, g, y}; //сохранение в начало файла

    mpz_class k, a, b, yk, m;
    for (unsigned char ch : text) {
        k = rng.get_z_range(p - 2) + 1; //случайный k
        m = ch;
        mpz_powm(a.get_mpz_t(), g.get_mpz_t(), k.get_mpz_t(), p.get_mpz_t()); //a = g^k mod p
        mpz_powm(yk.get_mpz_t(), y.get_mpz_t(), k.get_mpz_t(), p.get_mpz_t()); //y^k mod p
        b = (yk * m) % p;
        //запись зашифрованных данных
        encrypted.push_back(a);
        encrypted.push_back(b);
    }

    writeFile(outputFile, encrypted); //сохранение зашифрованных данных
    writeKey(keyFile, p, g, x); //сохранение ключа
}

//расшифрование файла
void elgamalDecryptFile(const char* inputFile, const char* keyFile, const char* outputFile) {
    vector<mpz_class> data;
    readFile(inputFile, data);

    mpz_class p, g, x;
    readKey(keyFile, p, g, x); //чтение ключа

    vector<unsigned char> decrypted;
    mpz_class a, b, s, s_inv, m;

    for (size_t i = 3; i < data.size(); i += 2) {
        a = data[i];
        b = data[i + 1];

        mpz_powm(s.get_mpz_t(), a.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t()); //s = a^x mod p
        mpz_invert(s_inv.get_mpz_t(), s.get_mpz_t(), p.get_mpz_t()); //s^-1 mod p
        m = (b * s_inv) % p;

        decrypted.push_back((unsigned char)(m.get_ui())); //преобразование в байт
    }

    writeFile(outputFile, decrypted); //запись расшифрованного файла
}

//шифрование текста
const char* elgamalEncryptText(const char* text, const char* keyFile) {
    static string result;
    result.clear();

    gmp_randclass rng(gmp_randinit_mt);
    rng.seed(time(NULL));

    //генерация параметров
    mpz_class p = generatePrime(32); 
    mpz_class g = findGenerator(p);             
    mpz_class x = rng.get_z_range(p - 2) + 1;
    mpz_class y; mpz_powm(y.get_mpz_t(), g.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t()); //открытый ключ

    for (const char* ptr = text; *ptr; ++ptr) {
        mpz_class m = (unsigned char)(*ptr); //перевод символа в число
        mpz_class k = rng.get_z_range(p - 2) + 1; //случайное k
        mpz_class a, b;

        mpz_powm(a.get_mpz_t(), g.get_mpz_t(), k.get_mpz_t(), p.get_mpz_t());
        mpz_class yk; mpz_powm(yk.get_mpz_t(), y.get_mpz_t(), k.get_mpz_t(), p.get_mpz_t());
        b = (yk * m) % p;

        char a_str[64], b_str[64];
        mpz_get_str(a_str, 16, a.get_mpz_t()); //перевод а в hex строку
        mpz_get_str(b_str, 16, b.get_mpz_t()); //перевод b в hex строку
        //перевод в формат a:b; для удобства работы
        result += a_str;
        result += ":";
        result += b_str;
        result += ";";
    }

    writeKey(keyFile, p, g, x); //сохранение ключа
    return result.c_str(); //возврат строки
}

//расшифование текста
const char* elgamalDecryptText(const char* ciphertext, const char* keyFile) {
    static string result;
    result.clear();

    mpz_class p, g, x;
    readKey(keyFile, p, g, x); //загрузка ключей

    istringstream ss(ciphertext); //разбор строки по ;
    string segment;

    while (getline(ss, segment, ';')) {
        if (segment.empty()) continue;
        size_t pos = segment.find(':');
        if (pos == string::npos) continue;

        string a_hex = segment.substr(0, pos);
        string b_hex = segment.substr(pos + 1);

        mpz_class a, b, s, s_inv, m;
        mpz_set_str(a.get_mpz_t(), a_hex.c_str(), 16); //преобразование из hex в число
        mpz_set_str(b.get_mpz_t(), b_hex.c_str(), 16);

        mpz_powm(s.get_mpz_t(), a.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t()); //s = a^x
        mpz_invert(s_inv.get_mpz_t(), s.get_mpz_t(), p.get_mpz_t()); //s^-1 = a^-x
        m = (b * s_inv) % p;

        result.push_back((unsigned char)m.get_ui()); //перевод в символ
    }

    return result.c_str(); //возврат текста
}

}