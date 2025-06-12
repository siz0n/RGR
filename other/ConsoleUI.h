#pragma once
#include "../crypto/CryptoService.h"
#include "../domain/CipherType.h"
#include "PluginLoader.h"
#include <cstdint>
#include <string>
#include <vector>

class ConsoleUI
{
  public:
    enum class OutputChoice : std::uint8_t
    {
        ShowOnly,
        SaveOnly,
        ShowAndSave
    };

    static int selectCipher(const std::vector<PluginInfo> &plugins, bool &exit);
    static bool selectOperation(bool &encrypt, bool &exit, bool &mainMenu);
    static bool inputIsBinary(bool &isBinary, bool &exit, bool &mainMenu);
    static bool inputKey(std::string &key, bool encrypt, CipherType cipherType, bool &exit, bool &mainMenu);
    static bool getText(std::string &text, const std::string &prompt, bool &exit, bool &mainMenu);
    static bool getBinary(std::vector<uint8_t> &data, const std::string &prompt, bool &exit, bool &mainMenu);
    static bool askOutputChoice(bool isBinary, OutputChoice &choice, bool &exit, bool &mainMenu);
    static bool askFilename(std::string &fname, bool isBinary, bool &exit, bool &mainMenu);
    static void outputResult(const std::string &result);
    static void outputBinaryResult(const std::vector<uint8_t> &result);
    static void printError(const std::string &message);
    static void pause();
    static bool handlePluginSelection(PluginLoader &loader, bool &exit);
    static bool handleCipherOperations(CryptoService &crypto, CipherType cipherType, bool &exit, bool &menu);
    static bool askMasterKey(std::string &key);
    static void processText(CryptoService &crypto, std::string &key, bool encrypt, CipherType cipherType, bool &exit, bool &mainMenu);
    static void processBinary(CryptoService &crypto, const std::string &key, bool encrypt, bool &exit, bool &mainMenu);
    static void mainLoop();
};