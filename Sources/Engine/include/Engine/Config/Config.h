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
    // A Simple class that allows for easy loading of ini files
    // Aswell as allow for getting values stored within the ini
    class ZEDENGINE_API Config
    {
    public:
        // Load an ini file from a specified path
        static bool Load(const std::string& iniPath);
        // Gets values stored within an ini file *use ZED::Config::Load to load an ini file first
        static const CSimpleIniA& Get();

    private:
        static std::unique_ptr<CSimpleIniA> s_ini;
    };
}

#endif
