#include "core/utils.hpp"

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#endif

using namespace std::chrono;

namespace vsrg {
    std::string Utils::getExecutableDir() {
        std::string path;

        // pulled this from another project, if it works it works. genuinely hate it though
#ifdef _WIN32
        char buffer[MAX_PATH];
        if (GetModuleFileNameA(NULL, buffer, MAX_PATH) > 0) {
            path = std::string(buffer);
        }
#elif defined(__linux__)
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count > 0) {
            path = std::string(buffer, count);
        }
#elif defined(__APPLE__)
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) == 0) {
            path = std::string(buffer);
        }
#endif

        if (path.empty()) return "./";

        std::string::size_type pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            path = path.substr(0, pos);
        }
        return path;
    }

    std::string Utils::getCurrentDate() {
        auto now = system_clock::now();
        auto now_time_t = system_clock::to_time_t(now);
        std::tm* local_time = std::localtime(&now_time_t);

        std::stringstream ss;
        ss << std::put_time(local_time, "%Y-%m-%d");
        return ss.str();
    }

    std::string Utils::getCurrentTimestamp(bool showMs) {
        auto now = system_clock::now();
        auto now_time_t = system_clock::to_time_t(now);

        auto ms = duration_cast<milliseconds>(now - system_clock::from_time_t(now_time_t));
        std::tm* local_time = std::localtime(&now_time_t);

        std::stringstream ss;
        ss << std::put_time(local_time, "%H:%M:%S");

        // this could be done better but whatever
        if (showMs) {
            ss << "." << std::setw(3) << std::setfill('0') << ms.count();
        }

        return ss.str();
    }
}