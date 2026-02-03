#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace vsrg
{
    template <typename... Args>
    std::string joinPaths(Args&&... args)
    {
        std::filesystem::path result;
        // why is this valid c++ syntax what the helly
        ((result /= std::forward<Args>(args)), ...);
        return result.string();
    }

    std::string getExecutableDir();
    std::string getAssetPath(std::string asset);

    std::string getCurrentDate();
    std::string getCurrentTimestamp(bool showMs = true);
}