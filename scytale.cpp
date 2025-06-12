#include "domain/ICipher.h"
#include "utf8.h"
#include <cstdint>
#include <ctime>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

static int generate_scytale_key(size_t len)
{
    if (len <= 2)
    {
        return 1;
    }
    std::mt19937 rng((unsigned int)std::time(nullptr));
    std::uniform_int_distribution<int> dist(2, (int)len - 1);
    int res = dist(rng) / 2;
    std::cout << "Сгенерированный ключ для Скиталы (столбцы): " << res << std::endl;
    return res;
}

std::wstring scytale(const std::wstring &text, int cols, bool encrypt)
{
    size_t len = text.size();
    if (cols <= 1 || cols >= (int)len)
        return text;
    size_t rows = (len + cols - 1) / cols;
    std::wstring result;
    if (encrypt)
    {
        for (int c = 0; c < cols; ++c)
        {
            for (size_t r = 0; r < rows; ++r)
            {
                size_t idx = (r * cols) + c;
                if (idx < len)
                {
                    result.push_back(text[idx]);
                }
            }
        }
    }
    else
    {
        size_t long_cols = len % cols;
        size_t base_len = len / cols;
        std::vector<size_t> col_lens(cols, base_len);
        for (size_t i = 0; i < long_cols; ++i)
        {
            col_lens[i]++;
        }
        std::vector<std::wstring> columns(cols);
        size_t pos = 0;
        for (int c = 0; c < cols; ++c)
        {
            columns[c] = text.substr(pos, col_lens[c]);
            pos += col_lens[c];
        }
        for (size_t r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (r < columns[c].size())
                {
                    result.push_back(columns[c][r]);
                }
            }
        }
    }
    return result;
}

#include <cstdint>
#include <vector>

static std::vector<unsigned char> scytale_bin(const std::vector<unsigned char> &data, int cols, bool encrypt)
{
    size_t len = data.size();
    if (cols <= 1 || cols >= static_cast<int>(len))
    {
        return data;
    }
    size_t rows = (len + cols - 1) / cols;
    std::vector<unsigned char> result;
    result.reserve(len);

    if (encrypt)
    {
        for (int c = 0; c < cols; ++c)
        {
            for (size_t r = 0; r < rows; ++r)
            {
                size_t idx = (r * cols) + c;
                if (idx < len)
                {
                    result.push_back(data[idx]);
                }
            }
        }
    }
    else
    {
        size_t long_cols = len % cols;
        size_t base_len = len / cols;
        std::vector<size_t> col_lens(cols, base_len);
        for (size_t i = 0; i < long_cols; ++i)
        {
            col_lens[i]++;
        }
        std::vector<std::vector<unsigned char>> columns(cols);
        size_t pos = 0;
        for (size_t c = 0; c < static_cast<size_t>(cols); ++c)
        {
            columns[c].assign(data.begin() + pos, data.begin() + pos + col_lens[c]);
            pos += col_lens[c];
        }
        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t c = 0; c < static_cast<size_t>(cols); ++c)
            {
                if (r < columns[c].size())
                {
                    result.push_back(columns[c][r]);
                }
            }
        }
    }
    return result;
}

class ScytaleCipher : public ICipher
{
  public:
    std::string encrypt(const std::string &text, const std::string &key) override
    {
        int cols = 0;
        std::wstring wtext;
        utf8::utf8to32(text.begin(), text.end(), std::back_inserter(wtext));
        if (key.empty())
        {
            cols = generate_scytale_key(wtext.size());
        }
        else
        {
            cols = parse_key(key);
        }
        std::wstring wres = scytale(wtext, cols, true);
        std::string result;
        utf8::utf32to8(wres.begin(), wres.end(), std::back_inserter(result));
        return result;
    }
    std::string decrypt(const std::string &text, const std::string &key) override
    {
        int cols = parse_key(key);
        std::wstring wtext;
        utf8::utf8to32(text.begin(), text.end(), std::back_inserter(wtext));
        std::wstring wres = scytale(wtext, cols, false);
        std::string result;
        utf8::utf32to8(wres.begin(), wres.end(), std::back_inserter(result));
        return result;
    }
    // --- Реализация для бинарных данных ---
    std::vector<uint8_t> encrypt(const std::vector<uint8_t> &data, const std::string &key)
    {
        int cols = 0;
        if (key.empty())
        {
            cols = generate_scytale_key(data.size());
        }
        else
        {
            cols = parse_key(key);
        }
        return scytale_bin(data, cols, true);
    }
    std::vector<uint8_t> decrypt(const std::vector<uint8_t> &data, const std::string &key)
    {
        int cols = parse_key(key);
        return scytale_bin(data, cols, false);
    }

  private:
    int parse_key(const std::string &key)
    {
        try
        {
            int val = std::stoi(key);
            if (val <= 0)
            {
                throw std::invalid_argument("ключи должны быть положительными");
            }
            return val;
        }
        catch (...)
        {
            throw std::invalid_argument("Error, не правильно введен ключ");
        }
    }
};

extern "C" ICipher *create() { return new ScytaleCipher(); }
