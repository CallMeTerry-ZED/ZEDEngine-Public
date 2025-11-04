/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef MODULELOADER_H
#define MODULELOADER_H

#pragma once

#include <string>
#include <unordered_map>
#include <Windows.h>

namespace ZED::Module
{
    class ZEDENGINE_API ModuleLoader
    {
    public:
        // Load modules listed under [Modules] in the ini file
        static void LoadModulesFromINI(const std::string& section = "Modules");

        // Get function pointer from a loaded module
        static FARPROC GetFunction(const std::string& moduleName, const std::string& functionName);

        // Free all loaded modules
        static void Cleanup();

    private:
        static inline std::unordered_map<std::string, HMODULE> s_modules;
    };
}

#endif
