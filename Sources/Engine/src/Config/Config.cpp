/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Config/Config.h"
#include <iostream>

namespace ZED
{

    std::unique_ptr<CSimpleIniA> Config::s_ini = nullptr;

    bool Config::Load(const std::string& iniPath)
    {
        s_ini = std::make_unique<CSimpleIniA>();
        s_ini->SetUnicode();

        SI_Error rc = s_ini->LoadFile(iniPath.c_str());
        if (rc < 0)
        {
            std::cerr << "[ZED::ZEDConfig] Failed to load INI file: " << iniPath << "\n";
            return false;
        }

        std::cout << "[ZED::ZEDConfig] INI file loaded: " << iniPath << "\n";
        return true;
    }

    const CSimpleIniA& Config::Get()
    {
        if (!s_ini) {
            std::cerr << "[ZED::ZEDConfig] Warning: INI file not loaded!\n";
            static CSimpleIniA dummy;
            return dummy;
        }
        return *s_ini;
    }
}