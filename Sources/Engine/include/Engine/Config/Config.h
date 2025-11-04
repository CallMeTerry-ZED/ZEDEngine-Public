/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H

#pragma once

#include <SimpleIni.h>
#include <string>
#include <memory>

namespace ZED
{
    class ZEDENGINE_API Config
    {
    public:
        static bool Load(const std::string& iniPath);
        static const CSimpleIniA& Get();

    private:
        static std::unique_ptr<CSimpleIniA> s_ini;
    };
}

#endif
