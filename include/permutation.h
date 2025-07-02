#ifndef PERMUTATION_H
#define PERMUTATION_H

#ifdef __cplusplus
extern "C" {
#endif

void permutationEncryptFile(const char* inputFile, const char* outputFile, const char* keyFile);
void permutationDecryptFile(const char* inputFile, const char* keyFile, const char* outputFile);

#ifdef __cplusplus
}
#endif

#endif
