#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <clocale>

#define DEBUG_MODE

struct Order
{
    int number;
    std::string clientName;
    std::string address;
    std::string items;
    int cost;
    std::string status;
};

Order orders[100];
int ordersCount = 0;

int toInt(std::string text)
{
    int number;

    try
    {
        number = std::stoi(text);
    }
    catch (const std::invalid_argument& e)
    {
        throw std::invalid_argument("Некорректный формат числа: " + text);
    }
    catch (const std::out_of_range& e)
    {
        throw std::out_of_range("Число вне допустимого диапазона: " + text);
    }

    if (number < 0)
    {
        throw std::out_of_range("Число не может быть отрицательным");
    }
    if (number > 1000000)
    {
        throw std::out_of_range("Число выходит за допустимые пределы");
    }

    return number;
}

int readNumber()
{
    std::string text;
    std::getline(std::cin, text);
    return toInt(text);
}

std::string takeField(std::string& line)
{
    std::size_t pos = line.find(";");
    std::string field = line.substr(0, pos);
    line = line.substr(pos + 1);
    return field;
}

void parseLine(std::string line, Order& order)
{
    int count = 0;
    int length = static_cast<int>(line.size());

    for (int i = 0; i < length; i++)
    {
        if (line[i] == ';')
        {
            count++;
        }
    }

    if (count < 5)
    {
        throw std::invalid_argument("Недостаточно данных в строке");
    }
    if (count > 5)
    {
        throw std::invalid_argument("Лишние данные в строке");
    }

    order.number = toInt(takeField(line));
    if (order.number == 0)
    {
        throw std::invalid_argument("Номер заказа должен быть больше нуля");
    }

    order.clientName = takeField(line);
    order.address = takeField(line);
    order.items = takeField(line);
    order.cost = toInt(takeField(line));
    order.status = line;

    if (order.status != "Ожидает" && order.status != "В пути" && order.status != "Доставлен")
    {
        throw std::invalid_argument("Неизвестный статус: " + order.status);
    }
}

void loadFromFile(std::string fileName)
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл " + fileName);
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        lineNumber++;

        if (line.size() > 0)
        {
            try
            {
                if (ordersCount >= 100)
                {
                    throw std::runtime_error("Слишком много заказов, максимум 100");
                }

                parseLine(line, orders[ordersCount]);
                ordersCount++;
            }
            catch (const std::exception& e)
            {
                std::cout << "Ошибка в строке " << lineNumber << ": " << e.what() << std::endl;
            }
        }
    }

    file.close();

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] Файл прочитан" << std::endl;
    std::cout << "[DEBUG] Загружено записей: " << ordersCount << std::endl;
#endif
}

void saveToFile(std::string fileName)
{
    std::ofstream file(fileName);

    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл для записи " + fileName);
    }

    for (int i = 0; i < ordersCount; i++)
    {
        file << orders[i].number << ";"
            << orders[i].clientName << ";"
            << orders[i].address << ";"
            << orders[i].items << ";"
            << orders[i].cost << ";"
            << orders[i].status << std::endl;
    }

    file.close();

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] Записано записей: " << ordersCount << std::endl;
#endif
}

void printOrder(Order order)
{
    std::cout << "Заказ №" << order.number << std::endl;
    std::cout << "  Клиент: " << order.clientName << std::endl;
    std::cout << "  Адрес: " << order.address << std::endl;
    std::cout << "  Состав: " << order.items << std::endl;
    std::cout << "  Стоимость: " << order.cost << std::endl;
    std::cout << "  Статус: " << order.status << std::endl;
}

int findIndex(int number)
{
    for (int i = 0; i < ordersCount; i++)
    {
        if (orders[i].number == number)
        {
            return i;
        }
    }
    return -1;
}

void findOrder()
{
    std::cout << "Введите номер заказа: ";
    int number = readNumber();

    int index = findIndex(number);

    if (index == -1)
    {
        std::cout << "Заказ не найден" << std::endl;
    }
    else
    {
        printOrder(orders[index]);
    }
}

void findByClient()
{
    std::string name;
    std::cout << "Введите фамилию клиента";
    std::getline(std::cin, name);

    int found = 0;

    for (int i = 0; i < ordersCount; i++)
    {
        std::size_t pos = orders[i].clientName.find(name);

        if (pos != std::string::npos)
        {
            printOrder(orders[i]);
            found = 1;
        }
    }

    if (found == 0)
    {
        std::cout << "Заказы клиента не найдены" << std::endl;
    }
}

void showWaiting()
{
    int found = 0;

    for (int i = 0; i < ordersCount; i++)
    {
        if (orders[i].status == "Ожидает")
        {
            printOrder(orders[i]);
            found = 1;
        }
    }

    if (found == 0)
    {
        std::cout << "Ожидающих заказов нет" << std::endl;
    }
}

void changeStatus()
{
    std::cout << "Введите номер заказа: ";
    int number = readNumber();

    int index = findIndex(number);

    if (index == -1)
    {
        throw std::runtime_error("Заказ с таким номером не найден");
    }

    if (orders[index].status == "Доставлен")
    {
        throw std::runtime_error("Заказ уже доставлен, статус менять нельзя");
    }

    std::cout << "1 - Ожидает" << std::endl;
    std::cout << "2 - В пути" << std::endl;
    std::cout << "3 - Доставлен" << std::endl;
    std::cout << "Новый статус: ";
    int choice = readNumber();

    if (choice == 1)
    {
        orders[index].status = "Ожидает";
    }
    else if (choice == 2)
    {
        orders[index].status = "В пути";
    }
    else if (choice == 3)
    {
        orders[index].status = "Доставлен";
    }
    else
    {
        throw std::invalid_argument("Неверный выбор статуса");
    }

    std::cout << "Статус изменён" << std::endl;
}

void showDeliveredSum()
{
    int sum = 0;

    for (int i = 0; i < ordersCount; i++)
    {
        if (orders[i].status == "Доставлен")
        {
            sum = sum + orders[i].cost;
        }
    }

    std::cout << "Стоимость доставленных заказов: " << sum << std::endl;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    std::string fileName = "orders.txt";

    try
    {
        loadFromFile(fileName);
    }
    catch (const std::exception& e)
    {
        std::cout << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    int choice = -1;

    while (choice != 0)
    {
        std::cout << std::endl;
        std::cout << "1 - Найти заказ по номеру" << std::endl;
        std::cout << "2 - Найти заказы клиента" << std::endl;
        std::cout << "3 - Заказы, ожидающие доставки" << std::endl;
        std::cout << "4 - Изменить статус заказа" << std::endl;
        std::cout << "5 - Стоимость доставленных заказов" << std::endl;
        std::cout << "6 - Сохранить изменения" << std::endl;
        std::cout << "0 - Выход" << std::endl;
        std::cout << "Ваш выбор: ";

        try
        {
            choice = readNumber();

            if (choice == 1)
            {
                findOrder();
            }
            else if (choice == 2)
            {
                findByClient();
            }
            else if (choice == 3)
            {
                showWaiting();
            }
            else if (choice == 4)
            {
                changeStatus();
            }
            else if (choice == 5)
            {
                showDeliveredSum();
            }
            else if (choice == 6)
            {
                saveToFile(fileName);
                std::cout << "Данные сохранены" << std::endl;
            }
            else if (choice != 0)
            {
                std::cout << "Нет такого пункта меню" << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Ошибка: " << e.what() << std::endl;
        }
    }

    return 0;
}