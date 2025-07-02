#include <iostream>
#include <dlfcn.h>
#include <limits>
#include <string>
using namespace std;

enum class PermAction { 
    ENCRYPT = 1, 
    DECRYPT = 2, 
    BACK = 3 
};
enum class PermSource { 
    TEXT = 1, 
    FILE = 2, 
    BACK = 3 
};

void permutationCiph() {
    void* lib = dlopen("./libpermutation.so", RTLD_LAZY);
    if (!lib) {
        cerr << "Ошибка загрузки libpermutation.so: " << dlerror() << endl;
        cout << "Нажмите Enter для продолжения...";
        cin.ignore();
        return;
    }

    typedef const char* (*EncryptTextFunc)(const char*, const char*);
    typedef const char* (*DecryptTextFunc)(const char*, const char*);
    typedef void (*EncryptFileFunc)(const char*, const char*, const char*);
    typedef void (*DecryptFileFunc)(const char*, const char*, const char*);

    EncryptTextFunc encryptText = (EncryptTextFunc)dlsym(lib, "permutationEncryptText");
    DecryptTextFunc decryptText = (DecryptTextFunc)dlsym(lib, "permutationDecryptText");
    EncryptFileFunc encryptFile = (EncryptFileFunc)dlsym(lib, "permutationEncryptFile");
    DecryptFileFunc decryptFile = (DecryptFileFunc)dlsym(lib, "permutationDecryptFile");

    if (!encryptText || !decryptText || !encryptFile || !decryptFile) {
        cerr << "Ошибка загрузки функций из DLL: " << dlerror() << endl;
        dlclose(lib);
        cout << "Нажмите Enter для продолжения...";
        cin.ignore();
        return;
    }

    while (true) {
        system("clear");
        cout << "=== БЛОЧНАЯ ПЕРЕСТАНОВКА ===" << endl;
        cout << "1. Зашифровать" << endl;
        cout << "2. Расшифровать" << endl;
        cout << "3. Назад" << endl;
        cout << "Выбор: ";

        int action;
        if (!(cin >> action) || action < 1 || action > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: введите число от 1 до 3.\n";
            cout << "Нажмите Enter для продолжения...";
            cin.get();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        PermAction perAction = static_cast<PermAction>(action);
        if (perAction == PermAction::BACK) break;

        cout << "1 - С консоли" << endl;
        cout << "2 - С файла" << endl;
        cout << "3 - Назад" << endl;
        cout << "Выбор: ";

        int source;
        if (!(cin >> source) || source < 1 || source > 3) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ошибка: Введите число от 1 до 3!\n";
                cout << "Нажмите Enter для продолжения...";
                cin.get();
                continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        PermSource src = static_cast<PermSource>(source);
        if (src == PermSource::BACK) continue;

        if (perAction == PermAction::ENCRYPT && src == PermSource::TEXT) {
            string text, keyFile;
            cout << "Введите текст для шифрования: ";
            getline(cin, text);
            if (text.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Файл для сохранения ключа: ";
            getline(cin, keyFile);
            if (keyFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            const char* result = encryptText(text.c_str(), keyFile.c_str());
            cout << "Зашифрованный текст:\n" << result << endl;
            cout << "Ключ сохранён в: " << keyFile << endl;
            cout << "Нажмите Enter для продолжения..."; cin.get();
        }
        else if (perAction == PermAction::DECRYPT && src == PermSource::TEXT) {
            string hexText, keyFile;
            cout << "Вставьте hex-строку: ";
            getline(cin, hexText);
            if (hexText.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Введите имя файла с ключом: ";
            getline(cin, keyFile);
            if (keyFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            const char* result = decryptText(hexText.c_str(), keyFile.c_str());
            cout << "Расшифрованный текст: " << result << endl;
            cout << "Нажмите Enter для продолжения..."; cin.get();
        }
        else if (perAction == PermAction::ENCRYPT && src == PermSource::FILE) {
            string inputFile, outputFile, keyFile;
            cout << "Файл для шифрования: "; 
            getline(cin, inputFile);
            if (inputFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Файл для сохранения: "; 
            getline(cin, outputFile);
            if (outputFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Файл ключа: "; 
            getline(cin, keyFile);
            if (keyFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            encryptFile(inputFile.c_str(), outputFile.c_str(), keyFile.c_str());
            cout << "Файл зашифрован. Нажмите Enter..."; cin.get();
        }
        else if (perAction == PermAction::DECRYPT && src == PermSource::FILE) {
            string inputFile, keyFile, outputFile;
            cout << "Зашифрованный файл: "; 
            getline(cin, inputFile);
            if (inputFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Файл ключа: "; 
            getline(cin, keyFile);
            if (keyFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Файл для сохранения: "; 
            getline(cin, outputFile);
            if (outputFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            decryptFile(inputFile.c_str(), keyFile.c_str(), outputFile.c_str());
            cout << "Файл расшифрован. Нажмите Enter..."; cin.get();
        }
    }
    dlclose(lib);
}
