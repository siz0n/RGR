#include "CryptoService.h"

CryptoService::CryptoService(std::unique_ptr<ICipher> cipher) : m_cipher(std::move(cipher)) {}

std::string CryptoService::encryptText(const std::string &plaintext, const std::string &key) { return m_cipher->encrypt(plaintext, key); }

std::string CryptoService::decryptText(const std::string &ciphertext, const std::string &key) { return m_cipher->decrypt(ciphertext, key); }

std::vector<uint8_t> CryptoService::encryptData(const std::vector<uint8_t> &data, const std::string &key) { return m_cipher->encrypt(data, key); }

std::vector<uint8_t> CryptoService::decryptData(const std::vector<uint8_t> &data, const std::string &key) { return m_cipher->decrypt(data, key); }