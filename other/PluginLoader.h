#pragma once
#include "../domain/CipherType.h"
#include "../domain/ICipher.h"
#include <memory>
#include <string>
#include <vector>

struct PluginInfo
{
    std::string m_name;
    CipherType m_type;
    std::string m_path;
};

class PluginLoader
{
  public:
    static std::vector<PluginInfo> findPlugins();
    static std::unique_ptr<ICipher> load(const PluginInfo &info);
};