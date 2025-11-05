/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Module/ModuleLoader.h"
#include "Engine/Config/Config.h"

#include <iostream>
#include <SimpleIni.h>

namespace ZED::Module
{
    void ModuleLoader::LoadModulesFromINI(const std::string& section)
    {
        const auto& ini = Config::Get();

        CSimpleIniA::TNamesDepend keys;
        ini.GetAllKeys(section.c_str(), keys);

        for (const auto& key : keys)
        {
            std::string moduleName = key.pItem;
            std::string dllPath = ini.GetValue(section.c_str(), moduleName.c_str(), "");

            if (dllPath.empty())
                continue;

            HMODULE handle = LoadLibraryA(dllPath.c_str());
            if (!handle)
            {
                std::cerr << "[ZED::ModuleLoader] Failed to load module: " << dllPath << "\n";
                continue;
            }

            std::cout << "[ZED::ModuleLoader] Loaded module [" << moduleName << "]: " << dllPath << "\n";
            s_modules[moduleName] = handle;
        }
    }

    FARPROC ModuleLoader::GetFunction(const std::string& moduleName, const std::string& functionName)
    {
        auto it = s_modules.find(moduleName);
        if (it == s_modules.end()) return nullptr;
        return GetProcAddress(it->second, functionName.c_str());
    }

    void ModuleLoader::Cleanup()
    {
        for (auto& [name, handle] : s_modules)
        {
            if (handle)
            {
                FreeLibrary(handle);
                std::cout << "[ZED::ModuleLoader] Unloaded module: " << name << "\n";
            }
        }
        s_modules.clear();
    }
}