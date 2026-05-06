#include <iostream>
#include <string>
#include <iomanip>
#include <format>
#include <sqlite3.h>
#include "tasks.h"

void init_db() {
    sqlite3* DB;
    char* errorMessage;

    if (sqlite3_open("hotel.db", &DB) != SQLITE_OK) {
        std::cerr << "Ошибка при открытии БД: " << sqlite3_errmsg(DB) << std::endl;
        return;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS RECORD("
        "ROOM INT PRIMARY KEY NOT NULL, "
        "NAME TEXT NOT NULL, "
        "AGE INT NOT NULL,"
        "NUMBER INT NOT NULL,"
        "DAYS INT NOT NULL,"
        "PRICE INT NOT NULL);";

    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка SQL: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
    sqlite3_close(DB);
}

void init_person() {
    sqlite3* DB;
    sqlite3_open("hotel.db", &DB);

    int days, age, number, room, price = 500;
    std::string name;

    std::cout << "Введите номер для брони (0-50 стандарт; 51-75 люкс; 76+ президентский): ";
    std::cin >> room;

    if (room > 100 || room < 0) {
        std::cout << "Комнаты не существует." << std::endl;
        std::cout << "\nНажмите Enter, чтобы продолжить...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }

    std::cin.ignore();
    std::cout << "Введите имя: ";
    std::getline(std::cin, name);
    std::cout << "Введите возраст: ";
    std::cin >> age;
    std::cout << "Введите номер телефона: ";
    std::cin >> number;
    std::cout << "Введите дни бронирования: ";
    std::cin >> days;

    if (room < 50 || room < 75) {
        price = price * 5 * days;
    }
    else if (50 <= room && room <= 75) {
        price = price * 10 * days;
    }
    else {
        price = price * 20 * days;
    }

    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO RECORD (ROOM, NAME, AGE, NUMBER, DAYS, PRICE) VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_prepare_v2(DB, sql, -1, &stmt, 0);

    sqlite3_bind_int(stmt, 1, room);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_int(stmt, 4, number);
    sqlite3_bind_int(stmt, 5, days);
    sqlite3_bind_int(stmt, 6, price);


    if (sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "Успех! Бронь успешно оформлена." << std::endl;
        std::cout << "\nНажмите Enter, чтобы продолжить...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }
    else {
        std::cout << "Возникла неизвестная ошибка: Не удалось оформить бронь.\nВозможно, этот номер уже занят." << std::endl;
        std::cout << "\nНажмите Enter, чтобы продолжить...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}

void delete_person() {
    sqlite3* DB;
    sqlite3_open("hotel.db", &DB);

    int room;

    std::cout << "Введите номер комнаты дл выселения (0-100): " << std::endl;
    std::cin >> room;
    std::cin.ignore();

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM RECORD WHERE ROOM = ?;";

    sqlite3_prepare_v2(DB, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, room);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(DB) > 0) {
            std::cout << "Успех! Бронь успешно удалена." << std::endl;
            std::cout << "\nНажмите Enter, чтобы продолжить...";
            std::cin.get();
        }
        else {
            std::cout << "Не удалось удалить бронь. Возможно, вы ввели комнату, в которую никто не заселён." << std::endl;
            std::cout << "\nНажмите Enter, чтобы продолжить...";
            std::cin.get();
        }
    }
    else {
        std::cout << "Возникла непредвиденная ошибка при попытке удалить бронь!" << std::endl;
        std::cout << "\nНажмите Enter, чтобы продолжить...";
        std::cin.get();
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}

void get_persons() {
    sqlite3* DB;
    if (sqlite3_open("hotel.db", &DB) != SQLITE_OK) {
        std::cerr << "Ошибка открытия БД" << std::endl;
        return;
    }

    const char* sql = "SELECT * FROM RECORD;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_close(DB);
        return;
    }

    std::cout << "\n" << std::left
        << std::setw(8) << "№ Комн."
        << std::setw(20) << "Имя гостя"
        << std::setw(8) << "Возраст"
        << std::setw(15) << "Телефон"
        << std::setw(8) << "Дни"
        << std::setw(12) << "Итого (руб)" << std::endl;

    std::cout << std::string(71, '-') << std::endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << std::left
            << std::setw(8) << sqlite3_column_int(stmt, 0)
            << std::setw(20) << (const char*)sqlite3_column_text(stmt, 1)
            << std::setw(8) << sqlite3_column_int(stmt, 2)
            << std::setw(15) << sqlite3_column_int(stmt, 3)
            << std::setw(8) << sqlite3_column_int(stmt, 4)
            << std::setw(12) << sqlite3_column_int(stmt, 5) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}

void food_price() {
    sqlite3* DB;
    int room, price = 250, new_price;
    std::string food;

    if (sqlite3_open("hotel.db", &DB) != SQLITE_OK) {
        std::cerr << "Ошибка открытия БД" << std::endl;
        return;
    }

    std::cin.ignore(1000, '\n');
    std::cout << "Что хотите заказать (breakfast/lunch/dinner): ";
    std::getline(std::cin, food);
    std::cout << "В какой номер доставить еду: ";
    std::cin >> room;

    if (food == "breakfast") {
        if (room < 50) price = price;
        else if (room <= 75) price = price * 5;
        else price = price * 7;
    }

    else if (food == "lunch") {
        if (room < 50) price = price * 9;
        else if (room <= 75) price = price * 13;
        else price = price * 17;
    }

    else if (food == "dinner") {
        if (room < 50) price = price * 20;
        else if (room <= 75) price = price * 22;
        else price = price * 24;
    } 
    
    else {
        std::cout << "Непредвиденная ошибка." << std::endl;
        return;
    }

    const char* sql2 = "SELECT PRICE FROM RECORD WHERE ROOM = ?;";
    sqlite3_stmt* stmt2;
    const char* sql = "UPDATE RECORD SET PRICE = ? WHERE ROOM = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_close(DB);
        return;
    }

    if (sqlite3_prepare_v2(DB, sql2, -1, &stmt2, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_close(DB);
        return;
    }

    sqlite3_bind_int(stmt2, 1, room);
    if (sqlite3_step(stmt2) == SQLITE_ROW) {
        new_price = sqlite3_column_int(stmt2, 0) + price;
    }

    sqlite3_bind_int(stmt, 2, room);
    sqlite3_bind_int(stmt, 1, new_price);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_finalize(stmt2);
    sqlite3_close(DB);

    std::cout << "Успешно! Заказ оформлен. Ваша стоимость проживания обновилась." << std::endl;
}

void update_person() {
    sqlite3* DB;
    int room, choice, number, days;
    std::string name;

    if (sqlite3_open("hotel.db", &DB) != SQLITE_OK) {
        std::cerr << "Ошибка открытия БД" << std::endl;
        return;
    }

    std::cout << "Введите номер: ";
    std::cin >> room;

    const char* sql = "UPDATE RECORD SET NAME = ?, NUMBER = ?, DAYS = ? WHERE ROOM = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_close(DB);
        return;
    }

    std::cin.ignore(1000, '\n');
    std::cout << "\nВведите новое имя: ";
    std::getline(std::cin, name);
    std::cout << "Введите новый номер телефона: ";
    std::cin >> number;
    std::cout << "Введите новое кол-во дней проживания: ";
    std::cin >> days;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, number);
    sqlite3_bind_int(stmt, 3, days);
    sqlite3_bind_int(stmt, 4, room);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Ошибка при обновлении: " << sqlite3_errmsg(DB) << std::endl;
        std::cout << "\nНажмите Enter, чтобы продолжить...";
        std::cin.ignore(1000, '\n');
        std::cin.get();
    } else {
        std::cout << "Успешно! Бронь перезаписана." << std::endl;
        std::cout << "\nНажмите Enter, чтобы продолжить...";
        std::cin.ignore(1000, '\n');
        std::cin.get();
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}

void get_room_info() {
    sqlite3* DB;
    int room;

    if (sqlite3_open("hotel.db", &DB) != SQLITE_OK) {
        std::cerr << "Ошибка открытия БД" << std::endl;
        return;
    }

    std::cout << "Введите номер комнаты(0-100): ";
    std::cin >> room;

    const char* sql = "SELECT * FROM RECORD WHERE ROOM = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_close(DB);
        return;
    }

    sqlite3_bind_int(stmt, 1, room);

    std::cout << "\n" << std::left
        << std::setw(8) << "№ Комн."
        << std::setw(20) << "Имя гостя"
        << std::setw(8) << "Возраст"
        << std::setw(15) << "Телефон"
        << std::setw(8) << "Дни"
        << std::setw(12) << "Итого (руб)" << std::endl;

    std::cout << std::string(71, '-') << std::endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << std::left
            << std::setw(8) << sqlite3_column_int(stmt, 0)
            << std::setw(20) << (const char*)sqlite3_column_text(stmt, 1)
            << std::setw(8) << sqlite3_column_int(stmt, 2)
            << std::setw(15) << sqlite3_column_int(stmt, 3)
            << std::setw(8) << sqlite3_column_int(stmt, 4)
            << std::setw(12) << sqlite3_column_int(stmt, 5) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}