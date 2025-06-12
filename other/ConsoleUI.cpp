#include "ConsoleUI.h"
#include "FileIO.h"
#include "PluginLoader.h"
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

namespace
{
std::string trim(const std::string &str) // удаление пробелов
{
    size_t start = str.find_first_not_of(" \n"); // поиск пробела в начале строки
    size_t end = str.find_last_not_of(" \n");    // поиск пробела в конце строки
    if (start == std::string::npos)              // проверка на пустую строку(npos -нету символов)
    {
        return "";
    }
    return str.substr(start, end - start + 1); // возвращает строку без пробелов
}
std::string readLine()
{
    std::string line;
    std::getline(std::cin, line);
    return trim(line);
}
bool isNumber(const std::string &str)
{
    for (char chr : str)
    {
        if (std::isdigit((unsigned char)chr) == 0) // проверка на символ
        {
            return false;
        }
    }
    return !str.empty();
}
std::string bytesToHex(const std::vector<uint8_t> &data, size_t maxBytes = 128)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < std::min(maxBytes, data.size()); ++i)
    {
        oss << std::setw(2) << (int)data[i];
    }
    return oss.str();
}
} // namespace

int ConsoleUI::selectCipher(const std::vector<PluginInfo> &plugins, bool &exit)
{
    exit = false;
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Примечание: для возврата в главное меню введите menu в любой момент.\n";
    std::cout << "---------------------------------------------------------------\n";

    if (plugins.empty())
    {
        std::cout << "Нет доступных плагинов шифрования!\n";
        exit = true;
        return -1;
    }
    while (true)
    {
        std::cout << "\n--- Доступные алгоритмы ---\n";
        for (size_t i = 0; i < plugins.size(); ++i)
        {
            std::cout << (i + 1) << ". " << plugins[i].m_name << "\n";
        }
        std::cout << "0. Выход\n";
        std::cout << "Введите номер: ";

        std::string choice = readLine();
        if (choice == "0")
        {
            exit = true;
            return -1;
        }
        if (choice == "menu")
        {
            exit = true;
            return -1;
        }
        if (isNumber(choice))
        {
            int idx = std::stoi(choice);
            if (idx >= 1 && idx <= (int)plugins.size())
            {
                return idx - 1;
            }
        }
        std::cout << "Ошибка: некорректный ввод. Повторите.\n";
    }
}

bool ConsoleUI::selectOperation(bool &encrypt, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    while (true)
    {
        std::cout << "\n--- Выберите операцию ---\n";
        std::cout << "1. Шифрование\n";
        std::cout << "2. Дешифрование\n";
        std::cout << "Введите номер: ";
        std::string choice = readLine();
        if (choice == "menu")
        {
            mainMenu = true;
            return false;
        }
        if (choice == "1")
        {
            encrypt = true;
            return true;
        }
        if (choice == "2")
        {
            encrypt = false;
            return true;
        }
        std::cout << "Ошибка: некорректный ввод. Повторите.\n";
    }
}

bool ConsoleUI::inputIsBinary(bool &isBinary, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    while (true)
    {
        std::cout << "\n--- Режим работы ---\n";
        std::cout << "1. Текст\n";
        std::cout << "2. Бинарные данные (файл)\n";
        std::cout << "Введите номер: ";
        std::string choice = readLine();
        if (choice == "menu")
        {
            mainMenu = true;
            return false;
        }
        if (choice == "1")
        {
            isBinary = false;
            return true;
        }
        if (choice == "2")
        {
            isBinary = true;
            return true;
        }
        std::cout << "Ошибка: некорректный ввод. Повторите.\n";
    }
}

bool ConsoleUI::inputKey(std::string &key, bool encrypt, CipherType cipherType, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    std::string operation = encrypt ? "шифрования" : "дешифрования";
    while (true)
    {
        std::cout << "\n--- Ввод ключа ---\n";
        std::cout << "Введите ключ для " << operation << " (или оставьте пустым для автогенерации).\n";
        if (cipherType == CipherType::Scytale)
        {
            std::cout << "Для Скиталы ключ — положительное целое число (количество столбцов).\n";
        }
        std::cout << "Ключ: ";
        key = readLine();
        if (key == "menu")
        {
            mainMenu = true;
            return false;
        }
        if (cipherType == CipherType::Scytale && !key.empty())
        {
            try
            {
                int val = std::stoi(key);
                if (val <= 0)
                {
                    throw std::invalid_argument("Ключ должен быть положительным числом");
                }
                break;
            }
            catch (...)
            {
                std::cout << "Ошибка: ключ для Скиталы должен быть положительным целым числом!\n";
                continue;
            }
        }
        break;
    }
    return true;
}

bool ConsoleUI::getText(std::string &text, const std::string &prompt, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    std::cout << "\n" << prompt << "\n";
    text = readLine();
    if (text == "menu")
    {
        mainMenu = true;
        return false;
    }
    return true;
}

bool ConsoleUI::getBinary(std::vector<uint8_t> &data, const std::string &prompt, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    while (true)
    {
        std::cout << "\n" << prompt << " \n";
        std::string fname = readLine();
        if (fname == "menu")
        {
            mainMenu = true;
            return false;
        }

        std::ifstream fin(fname, std::ios::binary);
        if (!fin)
        {
            std::cout << "Ошибка: не удалось открыть файл '" << fname << "'\n";
            continue; // спрашиваем снова
        }
        data.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
        return true;
    }
}

bool ConsoleUI::askOutputChoice(bool /*isBinary*/, OutputChoice &choice, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    while (true)
    {
        std::cout << "\n--- Как обработать результат? ---\n";
        std::cout << "1. Показать на экране\n";
        std::cout << "2. Сохранить в файл\n";
        std::cout << "3. И показать, и сохранить\n";
        std::cout << "Введите номер: ";
        std::string str = readLine();
        if (str == "menu")
        {
            mainMenu = true;
            return false;
        }
        if (str == "1")
        {
            choice = OutputChoice::ShowOnly;
            return true;
        }
        if (str == "2")
        {
            choice = OutputChoice::SaveOnly;
            return true;
        }
        if (str == "3")
        {
            choice = OutputChoice::ShowAndSave;
            return true;
        }
        std::cout << "Ошибка: некорректный ввод. Повторите.\n";
    }
}

bool ConsoleUI::askFilename(std::string &fname, bool /*isBinary*/, bool &exit, bool &mainMenu)
{
    exit = false;
    mainMenu = false;
    std::cout << "\nВведите имя файла для сохранения результата:\n";
    fname = readLine();
    if (fname == "menu")
    {
        mainMenu = true;
        return false;
    }
    return true;
}

void ConsoleUI::outputResult(const std::string &result)
{
    std::cout << "\n--- Результат ---\n";
    std::cout << result << "\n";
    std::cout << "-------------------\n";
}

void ConsoleUI::outputBinaryResult(const std::vector<uint8_t> &result)

{
    std::cout << "\n--- Результат (hex, первые 128 байт) ---\n ";
    std::cout << bytesToHex(result, 128) << "\n";
    std::cout << "------------------------------------------\n";
}

void ConsoleUI::printError(const std::string &message) { std::cout << "\n--- Ошибка ---\n" << message << "\n--------------\n"; }

void ConsoleUI::pause()
{
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool ConsoleUI::askMasterKey(std::string &key)
{
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Для продолжения работы введите пароль.\n";
    std::cout << "Для выхода введите пустую строку и нажмите Enter.\n";
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Введите пароль: ";
    std::string key1 = "123";
    std::getline(std::cin, key);
    if (key.empty() or key1 != key)
    {
        std::cout << "Вход отменён. Завершение работы.\n";
        return false;
    }
    return true;
}

void ConsoleUI::processText(CryptoService &crypto, std::string &key, bool encrypt, CipherType /*unused*/, bool &exit, bool &mainMenu)
{
    std::string inputText;
    if (!getText(inputText, encrypt ? "Введите текст для шифрования:" : "Введите текст для дешифрования:", exit, mainMenu))
    {
        return;
    }

    std::string result;
    try
    {
        if (encrypt)
        {
            result = crypto.encryptText(inputText, key);
        }
        else
        {
            result = crypto.decryptText(inputText, key);
        }
    }
    catch (const std::exception &e)
    {
        printError(e.what());
        pause();
        return;
    }

    OutputChoice outputChoice = OutputChoice::ShowOnly;
    if (!askOutputChoice(false, outputChoice, exit, mainMenu))
    {
        return;
    }

    if (outputChoice == OutputChoice::ShowOnly || outputChoice == OutputChoice::ShowAndSave)
    {
        outputResult(result);
    }

    if (outputChoice == OutputChoice::SaveOnly || outputChoice == OutputChoice::ShowAndSave)
    {
        std::string fname;
        if (!askFilename(fname, false, exit, mainMenu))
        {
            return;
        }
        try
        {
            FileIO::writeText(fname, result);
            std::cout << "Результат успешно сохранён в файл.\n";
        }
        catch (const std::exception &e)
        {
            printError(e.what());
            pause();
        }
    }
    pause();
}

void ConsoleUI::processBinary(CryptoService &crypto, const std::string &key, bool encrypt, bool &exit, bool &mainMenu)
{
    std::vector<uint8_t> data;
    if (!getBinary(data, encrypt ? "Введите имя файла для шифрования:" : "Введите имя файла для дешифрования:", exit, mainMenu))
    {
        return;
    }

    std::vector<uint8_t> result;
    try
    {
        if (encrypt)
        {
            result = crypto.encryptData(data, key);
        }
        else
        {
            result = crypto.decryptData(data, key);
        }
    }
    catch (const std::exception &e)
    {
        printError(e.what());
        pause();
        return;
    }

    OutputChoice outputChoice = OutputChoice::ShowOnly;
    if (!askOutputChoice(true, outputChoice, exit, mainMenu))
    {
        return;
    }

    if (outputChoice == OutputChoice::ShowOnly || outputChoice == OutputChoice::ShowAndSave)
    {
        outputBinaryResult(result);
    }

    if (outputChoice == OutputChoice::SaveOnly || outputChoice == OutputChoice::ShowAndSave)
    {
        std::string fname;
        if (!askFilename(fname, true, exit, mainMenu))
        {
            return;
        }
        try
        {
            FileIO::writeBinary(fname, result);
            std::cout << "Результат успешно сохранён в файл.\n";
        }
        catch (const std::exception &e)
        {
            printError(e.what());
            pause();
        }
    }
    pause();
}

bool ConsoleUI::handlePluginSelection(PluginLoader & /*loader*/, bool &exit)
{
    auto plugins = PluginLoader::findPlugins();
    bool menu = false;
    int pluginIndex = selectCipher(plugins, exit);
    if (exit)
    {
        return false;
    }

    const PluginInfo &selectedPlugin = plugins[pluginIndex];
    auto cipher = PluginLoader::load(selectedPlugin);
    if (!cipher)
    {
        printError("Не удалось загрузить плагин.");
        pause();
        return true;
    }
    CryptoService crypto(std::move(cipher));
    CipherType cipherType = selectedPlugin.m_type;

    return ConsoleUI::handleCipherOperations(crypto, cipherType, exit, menu);
}
bool ConsoleUI::handleCipherOperations(CryptoService &crypto, CipherType cipherType, bool &exit, bool &menu)
{
    bool encrypt = false;
    if (!selectOperation(encrypt, exit, menu))
    {
        if (exit)
        {
            return false;
        }
        if (menu)
        {
            return true;
        }
    }

    bool isBinary = false;
    if (!inputIsBinary(isBinary, exit, menu))
    {
        if (exit)
        {
            return false;
        }
        if (menu)
        {
            return true;
        }
    }

    std::string key;
    if (!inputKey(key, encrypt, cipherType, exit, menu))
    {
        if (exit)
        {
            return false;
        }
        if (menu)
        {
            return true;
        }
    }

    if (isBinary)
    {
        processBinary(crypto, key, encrypt, exit, menu);
    }
    else
    {
        processText(crypto, key, encrypt, cipherType, exit, menu);
    }
    return true;
}

void ConsoleUI::mainLoop()
{
    std::string masterKey;
    if (!askMasterKey(masterKey))
    {
        return;
    }

    PluginLoader loader;
    bool exit = false;

    while (!exit)
    {
        if (!handlePluginSelection(loader, exit))
        {
            break;
        }
    }
}