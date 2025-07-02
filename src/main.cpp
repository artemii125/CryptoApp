#include <iostream>
#include <limits>
#include <cstdlib>
#include "rc4_menu.h"
#include "elgamal_menu.h"
#include "permutation_menu.h"

using namespace std;

enum class CiphType {
    RC4 = 1,
    ELGAMAL = 2,
    PERMUTATION = 3,
    EXIT = 4
};

int main() {
    while (true) {
        try {
            system("clear");
            cout << "=== CryptoApp ===" << endl;
            cout << "1. RC4" << endl;
            cout << "2. Эль-Гамаль" << endl;
            cout << "3. Блочная перестановка" << endl;
            cout << "4. Выход" << endl;
            cout << "Выбор: ";

            int input;
            if (!(cin >> input) || input < 1 || input > 4) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Ошибка: Введите число от 1 до 4!");
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            CiphType choice = static_cast<CiphType>(input);
            switch (choice) {
                case CiphType::RC4:
                    rc4Ciph();
                    break;
                case CiphType::ELGAMAL:
                    elgamalCiph();
                    break;
                case CiphType::PERMUTATION:
                    permutationCiph();
                    break;
                case CiphType::EXIT:
                    cout << "Завершение программы..." << endl;
                    return 0;
            }
        } catch (const exception& e) {
            cerr << e.what() << endl;
            cout << "Нажмите Enter для продолжения...";
            cin.get();
        }
    }
}
