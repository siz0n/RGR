#include "domain/ICipher.h"
#include "utf8.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// Универсальная функция генерации ключа: печатные символы, кириллица, спецсимволы, цифры
static std::string generate_beaufort_key()
{
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)) ^ (uintptr_t)(new char));
    std::uniform_int_distribution<size_t> dist_len(8, 20);

    std::u32string alphabet = U"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              U"abcdefghijklmnopqrstuvwxyz"
                              U"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
                              U"абвгдежзийклмнопрстуфхцчшщъыьэюя"
                              U"0123456789"
                              U"!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~";
    std::uniform_int_distribution<size_t> dist_char(0, alphabet.size() - 1);

    size_t keylen = dist_len(rng);
    std::u32string key32;
    for (size_t i = 0; i < keylen; ++i)
        key32 += alphabet[dist_char(rng)];

    std::string key;
    utf8::utf32to8(key32.begin(), key32.end(), std::back_inserter(key));
    std::cout << "Сгенерированный ключ для Бофор: " << key << '\n';
    return key;
}

class BeaufortCipher : public ICipher
{
  public:
    // Текстовый режим (шифрует строку, возвращает hex)
    std::string encrypt(const std::string &text, const std::string &key) override
    {
        std::string used_key = key;
        if (key.empty())
            used_key = generate_beaufort_key();
        return to_hex(process(text, used_key));
    }

    std::string decrypt(const std::string &text, const std::string &key) override { return process(from_hex(text), key); }

    // Бинарный режим (шифрует любые данные)
    std::vector<uint8_t> encrypt(const std::vector<uint8_t> &data, const std::string &key) override
    {
        std::string used_key = key;
        if (key.empty())
            used_key = generate_beaufort_key();
        std::vector<uint8_t> res(data.size());
        for (size_t i = 0; i < data.size(); ++i)
        {
            uint8_t k = used_key.empty() ? 0 : static_cast<uint8_t>(used_key[i % used_key.size()]);
            res[i] = data[i] ^ k;
        }
        return res;
    }

    std::vector<uint8_t> decrypt(const std::vector<uint8_t> &data, const std::string &key) override
    {
        // Для Бофора операция симметрична (XOR)
        return encrypt(data, key);
    }

  private:
    static std::u32string process(const std::u32string &wtext, const std::u32string &wkey)
    {
        std::u32string result;
        for (size_t i = 0; i < wtext.size(); ++i)
        {
            char32_t t = wtext[i];
            char32_t k = wkey.empty() ? U' ' : wkey[i % wkey.size()];
            char32_t c = (k >= t) ? (k - t) : (0x110000 + k - t);
            c %= 0x110000;
            result.push_back(c);
        }
        return result;
    }

    static std::string process(const std::string &text, const std::string &key)
    {
        std::u32string wtext;
        std::u32string wkey;
        utf8::utf8to32(text.begin(), text.end(), std::back_inserter(wtext));
        utf8::utf8to32(key.begin(), key.end(), std::back_inserter(wkey));
        std::u32string res = process(wtext, wkey);
        std::string out;
        utf8::utf32to8(res.begin(), res.end(), std::back_inserter(out));
        return out;
    }

    static std::string to_hex(const std::string &input)
    {
        std::ostringstream oss;
        for (unsigned char c : input)
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        return oss.str();
    }

    static std::string from_hex(const std::string &hex)
    {
        std::string output;
        for (size_t i = 0; i < hex.length(); i += 2)
        {
            std::string byteString = hex.substr(i, 2);
            char chr = (char)(int)strtol(byteString.c_str(), nullptr, 16);
            output.push_back(chr);
        }
        return output;
    }
};

extern "C" ICipher *create() { return new BeaufortCipher(); }