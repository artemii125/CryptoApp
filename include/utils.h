#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <gmpxx.h>

using namespace std;

vector<unsigned char> readFile(const char* filename);
void writeFile(const char* filename, const vector<unsigned char>& data);
void readFile(const char* filename, vector<mpz_class>& data);
void writeFile(const char* filename, const vector<mpz_class>& data);
void writeKey(const char* filename, const mpz_class& p, const mpz_class& g, const mpz_class& x);
void readKey(const char* filename, mpz_class& p, mpz_class& g, mpz_class& x);
void waitForKey();

#endif
