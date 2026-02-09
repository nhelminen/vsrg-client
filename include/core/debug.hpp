#pragma once

#include <algorithm>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>


namespace vsrg {
enum class DebugLevel { NONE, ERROR, WARNING, INFO, DEBUG };

class Debugger {
public:
    Debugger();
    ~Debugger();

    virtual void log(DebugLevel level, const std::string& message, const char* file, int line);

private:
    bool saveToFile = true;
    bool running = true;

    std::string levelToString(DebugLevel level);

    std::ofstream logFile;
    std::mutex logMutex;

    std::queue<std::string> logQueue;
    std::condition_variable logNotify;
    std::thread logThread;

    void processQueue();
};
}  // namespace vsrg

// create a macro so i can catch file and line automatically
#define VSRG_LOG(debugger, level, message) (debugger).log(level, message, __FILE__, __LINE__)