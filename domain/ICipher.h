#pragma once
#include <cstdint>
#include <string>
#include <vector>

class ICipher
{
  public:
    virtual ~ICipher() = default;
    // Текстовые методы
    virtual std::string encrypt(const std::string &plaintext, const std::string &key) = 0;
    virtual std::string decrypt(const std::string &ciphertext, const std::string &key) = 0;
    // Методы для бинарных данных
    virtual std::vector<uint8_t> encrypt(const std::vector<uint8_t> &data, const std::string &key) = 0;
    virtual std::vector<uint8_t> decrypt(const std::vector<uint8_t> &data, const std::string &key) = 0;
};