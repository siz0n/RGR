#include "domain/ICipher.h"
#include <cstdint>
#include <ctime>
#include <gmpxx.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
class RSACipher : public ICipher
{
  public:
    mpz_class e, d, n;
    bool generate_mode = false;
    RSACipher() = default;
    void generate_keys(int bits = 512)
    {
        gmp_randclass rng(gmp_randinit_default);
        rng.seed(static_cast<unsigned long>(time(nullptr)));
        mpz_class p;
        mpz_class q;
        mpz_class phi;
        do
        {
            p = rng.get_z_bits(bits / 2);
            mpz_nextprime(p.get_mpz_t(), p.get_mpz_t());
            q = rng.get_z_bits(bits / 2);
            mpz_nextprime(q.get_mpz_t(), q.get_mpz_t());
        } while (p == q);
        n = p * q;
        phi = (p - 1) * (q - 1);
        e = 65537;
        while (mpz_gcd_ui(nullptr, phi.get_mpz_t(), e.get_ui()) != 1 && e < phi)
        {
            e += 2;
        }
        if (!mpz_invert(d.get_mpz_t(), e.get_mpz_t(), phi.get_mpz_t()))
        {
            std::cerr << "Ошибка: не удалось найти обратный элемент для e по модулю phi\n";
            e = d = n = 0;
        }
    }
    void set_public_key(const std::string &key)
    {
        std::istringstream iss(key);
        std::string estr;
        std::string nstr;
        iss >> estr >> nstr;
        e.set_str(estr, 10);
        n.set_str(nstr, 10);
    }
    void set_private_key(const std::string &key)
    {
        std::istringstream iss(key);
        std::string dstr;
        std::string nstr;
        iss >> dstr >> nstr;
        d.set_str(dstr, 10);
        n.set_str(nstr, 10);
    }
    std::string encrypt(const std::string &text, const std::string &key) override
    {
        if (key.empty())
        {
            generate_mode = true;
            generate_keys();
            std::cout << "\nСгенерированные ключи RSA:\n";
            std::cout << "Открытый ключ (e n): " << e.get_str() << " " << n.get_str() << "\n";
            std::cout << "Закрытый ключ (d n): " << d.get_str() << " " << n.get_str() << "\n";
            std::cout << "(Сохраните закрытый ключ для расшифровки!)\n";
            std::cout << "----------------------------------\n";
        }
        else
        {
            set_public_key(key);
        }
        std::ostringstream oss;
        for (unsigned char c : text)
        {
            mpz_class m((int)c);
            mpz_class ciph;
            mpz_powm(ciph.get_mpz_t(), m.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
            oss << ciph.get_str() << " ";
        }
        return oss.str();
    }
    std::string decrypt(const std::string &text, const std::string &key) override
    {
        if (key.empty())
        {
            std::cerr << "Ошибка: для расшифровки введите закрытый ключ (d n)\n";
            return "";
        }
        set_private_key(key);
        std::istringstream iss(text);
        std::ostringstream oss;
        std::string block;
        while (iss >> block)
        {
            mpz_class ciph;
            try
            {
                ciph.set_str(block, 10);
            }
            catch (...)
            {
                std::cerr << "Ошибка: некорректный блок данных в шифротексте.\n";
                return "";
            }
            mpz_class m;
            mpz_powm(m.get_mpz_t(), ciph.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
            unsigned long v = m.get_ui();
            oss << static_cast<char>(v);
        }
        return oss.str();
    }

    std::vector<uint8_t> encrypt(const std::vector<uint8_t> &data, const std::string &key)
    {
        if (key.empty())
        {
            generate_mode = true;
            generate_keys();
            std::cout << "\nСгенерированные ключи RSA:\n";
            std::cout << "Открытый ключ (e n): " << e.get_str() << " " << n.get_str() << "\n";
            std::cout << "Закрытый ключ (d n): " << d.get_str() << " " << n.get_str() << "\n";
            std::cout << "(Сохраните закрытый ключ для расшифровки!)\n";
            std::cout << "----------------------------------\n";
        }
        else
        {
            set_public_key(key);
        }

        std::vector<uint8_t> result;
        for (uint8_t b : data)
        {
            mpz_class m((unsigned long)b);
            mpz_class ciph;
            mpz_powm(ciph.get_mpz_t(), m.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
            std::string ciph_str = ciph.get_str(16); // hex
            uint32_t len = ciph_str.size();
            result.push_back((len >> 24) & 0xFF);
            result.push_back((len >> 16) & 0xFF);
            result.push_back((len >> 8) & 0xFF);
            result.push_back(len & 0xFF);
            for (char ch : ciph_str)
            {
                result.push_back((uint8_t)ch);
            }
        }
        return result;
    }
    std::vector<uint8_t> decrypt(const std::vector<uint8_t> &data, const std::string &key)
    {
        if (key.empty())
        {
            std::cerr << "Ошибка: для расшифровки введите закрытый ключ (d n)\n";
            return {};
        }
        set_private_key(key);
        std::vector<uint8_t> result;
        size_t i = 0;
        while (i + 4 <= data.size())
        {
            uint32_t len = (data[i] << 24) | (data[i + 1] << 16) | (data[i + 2] << 8) | (data[i + 3]);
            i += 4;
            if (i + len > data.size())
            {
                break;
            }
            std::string ciph_str(data.begin() + i, data.begin() + i + len);
            mpz_class ciph;
            ciph.set_str(ciph_str, 16);
            mpz_class m;
            mpz_powm(m.get_mpz_t(), ciph.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
            result.push_back((uint8_t)m.get_ui());
            i += len;
        }
        return result;
    }
};

extern "C" ICipher *create() { return new RSACipher(); }
