#include "other/ConsoleUI.h"
#include <clocale>

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    try
    {
        ConsoleUI::mainLoop();
    }
    catch (const std::exception &e)
    {
        ConsoleUI::printError(std::string("Фатальная ошибка: ") + e.what());
        ConsoleUI::pause();
        return 1;
    }
    catch (...)
    {
        ConsoleUI::printError("Неизвестная критическая ошибка!");
        ConsoleUI::pause();
        return 2;
    }
    return 0;
}