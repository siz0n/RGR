#pragma once
#include "../domain/ICipher.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class CryptoService
{
  public:
    explicit CryptoService(std::unique_ptr<ICipher> cipher);

    std::string encryptText(const std::string &plaintext, const std::string &key);
    std::string decryptText(const std::string &ciphertext, const std::string &key);

    std::vector<uint8_t> encryptData(const std::vector<uint8_t> &data, const std::string &key);
    std::vector<uint8_t> decryptData(const std::vector<uint8_t> &data, const std::string &key);

  private:
    std::unique_ptr<ICipher> m_cipher;
};