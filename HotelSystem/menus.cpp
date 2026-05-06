#include <iostream>
#include "tasks.h"
using namespace std;

void menu1() {
    setlocale(LC_ALL, "Russian");
    int choice;

    while (true) {
        system("cls");
        cout << "===========БРОНИРОВАНИЕ НОМЕРА===========" << endl;
        cout << "1. Забронировать номер" << endl;
        cout << "2. Выселить из номера" << endl;
        cout << "3. Перезаписать бронь" << endl;
        cout << "0. Вернуться в главное меню" << endl;
        cout << "=========================================" << endl;
        cout << "Ваш выбор: ";

        cin >> choice;

        switch (choice) {
        case 1:
            system("cls");
            init_person(); break;
        case 2:
            system("cls");
            delete_person(); break;
        case 3:
            system("cls");
            update_person(); break;
        case 0:
            system("cls");
            return;
        }
    }
}