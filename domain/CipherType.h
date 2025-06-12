#pragma once
#include <cstdint>
#include <string>

enum class CipherType : std::uint8_t
{
    Scytale = 0,
    Beaufort = 1,
    RSA = 2
};
inline std::string cipherTypeToString(CipherType type)
{
    switch (type)
    {
    case CipherType::RSA:
        return "RSA";
    case CipherType::Scytale:
        return "Скитала";
    case CipherType::Beaufort:
        return "Бофор";
    default:
        return "";
    }
}