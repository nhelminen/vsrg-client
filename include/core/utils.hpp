#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace vsrg
{
    class Utils
    {
    public:
        template <typename... Args>
        static std::string joinPaths(Args&&... args)
        {
            std::filesystem::path result;
            // why is this valid c++ syntax what the helly
            ((result /= std::forward<Args>(args)), ...);
            return result.string();
        }
        static std::string getExecutableDir();
        static std::string getCurrentDate();
        static std::string getCurrentTimestamp(bool showMs = true);
    };
}