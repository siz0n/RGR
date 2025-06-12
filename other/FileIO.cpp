#include "FileIO.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace FileIO
{
std::vector<uint8_t> readBinary(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    file.seekg(0, std::ios::end);
    std::streamsize length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> tempBuffer(length);
    if (!file.read(tempBuffer.data(), length))
    {
        throw std::runtime_error("Ошибка при чтении файла: " + filename);
    }

    std::vector<uint8_t> buffer(length);
    std::memcpy(buffer.data(), tempBuffer.data(), static_cast<size_t>(length));
    return buffer;
}

void writeBinary(const std::string &filename, const std::vector<uint8_t> &data)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
    }

    std::vector<char> tempBuffer(data.size());
    std::memcpy(tempBuffer.data(), data.data(), data.size());
    file.write(tempBuffer.data(), static_cast<std::streamsize>(tempBuffer.size()));
}

void writeText(const std::string &filename, const std::string &text)
{
    std::ofstream file(filename);
    file << text;
}
} // namespace FileIO