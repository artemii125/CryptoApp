#ifndef ELGAMAL_H
#define ELGAMAL_H

#ifdef __cplusplus
#include <string>

using namespace std;

extern "C" {
#endif

void elgamalEncryptFile(const char* inputFile, const char* outputFile, const char* keyFile);
void elgamalDecryptFile(const char* inputFile, const char* keyFile, const char* outputFile);

#ifdef __cplusplus
}
string elgamalEncryptText(const string& text, const string& keyFile);
string elgamalDecryptText(const string& ciphertext, const string& keyFile);
#endif

#endif
