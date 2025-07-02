#ifndef RC4_H
#define RC4_H

#ifdef __cplusplus
#include <string>

using namespace std;

extern "C" {
#endif

void rc4EncryptFile(const char* inputFile, const char* outputFile, const char* keyFile);
void rc4DecryptFile(const char* inputFile, const char* keyFile, const char* outputFile);

#ifdef __cplusplus
}
string rc4EncryptText(const string& text, const string& keyFile);
string rc4DecryptText(const string& ciphertext, const string& keyFile);
#endif

#endif