#include <iostream>
#include <dlfcn.h>
#include <limits>
#include <string>
using namespace std;

enum class ElgAction { 
    ENCRYPT = 1, 
    DECRYPT = 2, 
    BACK = 3 
};
enum class ElgSource { 
    TEXT = 1, 
    FILE = 2,
    BACK = 3 
};

void elgamalCiph() {
    void* lib = dlopen("./libelgamal.so", RTLD_LAZY);
    if (!lib) {
        cerr << "Ошибка загрузки libelgamal.so: " << dlerror() << endl;
        cout << "Нажмите Enter для продолжения...";
        cin.ignore();
        return;
    }

    typedef const char* (*EncryptTextFunc)(const char*, const char*);
    typedef const char* (*DecryptTextFunc)(const char*, const char*);
    typedef void (*EncryptFileFunc)(const char*, const char*, const char*);
    typedef void (*DecryptFileFunc)(const char*, const char*, const char*);

    EncryptTextFunc encryptText = (EncryptTextFunc)dlsym(lib, "elgamalEncryptText");
    DecryptTextFunc decryptText = (DecryptTextFunc)dlsym(lib, "elgamalDecryptText");
    EncryptFileFunc encryptFile = (EncryptFileFunc)dlsym(lib, "elgamalEncryptFile");
    DecryptFileFunc decryptFile = (DecryptFileFunc)dlsym(lib, "elgamalDecryptFile");

    if (!encryptText || !decryptText || !encryptFile || !decryptFile) {
        cerr << "Ошибка загрузки функций из DLL: " << dlerror() << endl;
        dlclose(lib);
        cout << "Нажмите Enter для продолжения...";
        cin.ignore();
        return;
    }

    while (true) {
        system("clear");
        cout << "======= ЭЛЬ-ГАМАЛЬ =======" << endl;
        cout << "1 - Зашифровать" << endl;
        cout << "2 - Расшифровать" << endl;
        cout << "3 - Назад" << endl;
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

        ElgAction elgAction = static_cast<ElgAction>(action);
        if (elgAction == ElgAction::BACK) break;

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

        ElgSource src = static_cast<ElgSource>(source);
        if (src == ElgSource::BACK) continue;

        if (elgAction == ElgAction::ENCRYPT && src == ElgSource::TEXT) {
            string text, keyFile;
            cout << "Введите текст для шифрования: ";
            getline(cin, text);
            if (text.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            cout << "Введите имя файла для сохранения ключа: ";
            getline(cin, keyFile);
            if (keyFile.empty()) {
                cout << "Пустой ввод. Отмена.\n";
                continue;
            }
            const char* encrypted = encryptText(text.c_str(), keyFile.c_str());
            cout << "Зашифрованный текст:\n" << encrypted << endl;
            cout << "Ключ сохранён в файл: " << keyFile << endl;
            cout << "Нажмите Enter для продолжения..."; cin.get();
        }
        else if (elgAction == ElgAction::DECRYPT && src == ElgSource::TEXT) {
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
            const char* decrypted = decryptText(hexText.c_str(), keyFile.c_str());
            cout << "Расшифрованный текст: " << decrypted << endl;
            cout << "Нажмите Enter для продолжения..."; cin.get();
        }
        else if (elgAction == ElgAction::ENCRYPT && src == ElgSource::FILE) {
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
        else if (elgAction == ElgAction::DECRYPT && src == ElgSource::FILE) {
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
