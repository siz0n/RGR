#include "PluginLoader.h"
#include <dlfcn.h>
#include <filesystem>
#include <iostream>

std::vector<PluginInfo> PluginLoader::findPlugins()
{
    std::vector<PluginInfo> plugins;
    namespace fs = std::filesystem;
    if (fs::exists(".") && fs::is_directory("."))
    {
        for (const auto &entry : fs::directory_iterator("."))
        {
            if (entry.path().extension() == ".so")
            {
                if (entry.path().filename().string() == "rsa.so")
                {
                    plugins.push_back({"RSA (асимметричное шифрование)", CipherType::RSA, entry.path()});
                }
                else if (entry.path().filename().string() == "scytale.so")
                {
                    plugins.push_back({"Скитала (простая перестановка)", CipherType::Scytale, entry.path()});
                }
                else if (entry.path().filename().string() == "beaufort.so")
                {
                    plugins.push_back({"Бофор (полиалфавитный шифр)", CipherType::Beaufort, entry.path()});
                }
            }
        }
    }
    return plugins;
}

std::unique_ptr<ICipher> PluginLoader::load(const PluginInfo &info)
{
    void *handle = dlopen(info.m_path.c_str(), RTLD_LAZY);
    if (handle == nullptr)
    {
        std::cerr << "Не удалось открыть библиотеку: " << dlerror() << '\n';
        return nullptr;
    }
    using CreateFunc = ICipher *(*)();
    auto createFunc = reinterpret_cast<CreateFunc>(dlsym(handle, "create"));
    if (createFunc == nullptr)
    {
        std::cerr << "Невозможно загрузить создание символа: " << dlerror() << '\n';
        dlclose(handle);
        return nullptr;
    }
    return std::unique_ptr<ICipher>(createFunc());
}