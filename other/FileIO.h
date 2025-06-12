#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace FileIO
{
std::vector<uint8_t> readBinary(const std::string &filename);
void writeBinary(const std::string &filename, const std::vector<uint8_t> &data);
void writeText(const std::string &filename, const std::string &text);
} // namespace FileIO