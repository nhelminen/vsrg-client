#include "core/utils.hpp"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>

#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>

#include <fstream>

#elif defined(__APPLE__)
#include <limits.h>
#include <mach-o/dyld.h>
#include <mach/mach.h>

#endif

using namespace std::chrono;

namespace vsrg {
std::string getExecutableDir() {
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

std::string getAssetPath(std::string asset) {
    std::string execDir = getExecutableDir();
    return joinPaths(execDir, "assets", asset);
}

std::string getCurrentDate() {
    auto now = system_clock::now();
    auto now_time_t = system_clock::to_time_t(now);

    std::tm local_time;
#ifdef _WIN32
    localtime_s(&local_time, &now_time_t);
#else
    localtime_r(&now_time_t, &local_time);
#endif

    std::stringstream ss;
    ss << std::put_time(&local_time, "%Y-%m-%d");
    return ss.str();
}

std::string getCurrentTimestamp(bool showMs) {
    auto now = system_clock::now();
    auto now_time_t = system_clock::to_time_t(now);

    std::tm local_time;
#ifdef _WIN32
    localtime_s(&local_time, &now_time_t);
#else
    localtime_r(&now_time_t, &local_time);
#endif

    auto ms = duration_cast<milliseconds>(now - system_clock::from_time_t(now_time_t));

    std::stringstream ss;
    ss << std::put_time(&local_time, "%H:%M:%S");

    if (showMs) {
        ss << "." << std::setw(3) << std::setfill('0') << ms.count();
    }

    return ss.str();
}

float getFPS(float deltaTime) {
    if (deltaTime <= 0.0) return 0.0;

    return 1.0 / deltaTime;
}

size_t getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc))) {
        return static_cast<size_t>(pmc.WorkingSetSize);
    }
    return 0;
#elif defined(__linux__)
    std::ifstream status_file("/proc/self/status");
    std::string line;

    while (std::getline(status_file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            size_t memory_kb = 0;
            std::stringstream ss(line.substr(6));
            ss >> memory_kb;
            return memory_kb * 1024;
        }
    }
    return 0;
#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) ==
        KERN_SUCCESS) {
        return static_cast<size_t>(info.resident_size);
    }
    return 0;
#else
    return 0;
#endif
}

std::string getFormattedMemoryUsage() {
    size_t bytes = getMemoryUsage();
    double mb = static_cast<double>(bytes) / (1024.0 * 1024.0);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << mb << " MB";
    return ss.str();
}
}  // namespace vsrg