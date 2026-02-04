#include "core/debug.hpp"
#include "core/utils.hpp"

#include <sstream>

namespace vsrg
{
    Debugger::Debugger()
    {
        std::string execDir = getExecutableDir();
        std::string logDir = joinPaths(execDir, "logs");

        try
        {
            if (!std::filesystem::exists(logDir))
            {
                std::filesystem::create_directory(logDir);
            }
        }
        catch (...)
        {
            saveToFile = false; // disable file saving if it cant access or create log dir
        }

        std::string curDate = getCurrentDate();
        std::string curTime = getCurrentTimestamp(false);

        std::string fileName = "debug_" + curDate + "_" + curTime + ".log";

        // clean up filename, lazy method
        for (auto &ch : fileName)
        {
            if (ch == ':' || ch == ' ')
            {
                ch = '-';
            }
        }

        std::string logPath = joinPaths(logDir, fileName);
        if (saveToFile)
        {
            logFile.open(logPath, std::ios::out | std::ios::app);
            if (!logFile.is_open())
            {
                saveToFile = false; // disable if failed to open
            }
        }

        logThread = std::thread(&Debugger::processQueue, this);
    }

    Debugger::~Debugger()
    {
        {
            std::lock_guard<std::mutex> lock(logMutex);
            if (running)
            {
                running = false;
            }
        }
        logNotify.notify_one();
        if (logThread.joinable())
        {
            logThread.join();
        }

        {
            std::lock_guard<std::mutex> lock(logMutex);
            while (!logQueue.empty())
            {
                std::string message = logQueue.front();
                logQueue.pop();

                if (message.find("[ERROR]") != std::string::npos)
                {
                    std::cerr << message << std::endl;
                }
                else
                {
                    std::cout << message << std::endl;
                }

                if (saveToFile && logFile.is_open())
                {
                    logFile << message << std::endl;
                }
            }
        }

        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    std::string Debugger::levelToString(DebugLevel level)
    {
        switch (level)
        {
        case DebugLevel::INFO:
            return "[INFO]";
        case DebugLevel::WARNING:
            return "[WARN]";
        case DebugLevel::ERROR:
            return "[ERROR]";
        case DebugLevel::DEBUG:
            return "[DEBUG]";
        default:
            return "[UNKWN]";
        }
    }

    void Debugger::log(DebugLevel level, const std::string &message, const char *file, int line)
    {
        std::string prefix = levelToString(level);
        std::string timestamp = getCurrentTimestamp();

        std::filesystem::path fullPath(file);
        std::filesystem::path rootPath(VSRG_PROJECT_ROOT);

        std::string displayPath;
        try {
            displayPath = std::filesystem::relative(fullPath, rootPath).string();
            std::replace(displayPath.begin(), displayPath.end(), '\\', '/');
        } catch (...) {
            displayPath = fullPath.filename().string();
        }

        std::stringstream logOutput;
        logOutput << timestamp << " " << prefix;
        logOutput << " [" << displayPath << ":" << line << "]";
        logOutput << " " << message;

        std::string finalLog = logOutput.str();

        {
            std::lock_guard<std::mutex> lock(logMutex);
            logQueue.push(finalLog);
        }
        logNotify.notify_one();
    }

    void Debugger::processQueue()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(logMutex);
            logNotify.wait(lock, [this]
                           { return !logQueue.empty() || !running; });

            if (!running && logQueue.empty())
            {
                break;
            }

            while (!logQueue.empty())
            {
                std::string message = logQueue.front();
                logQueue.pop();

                // log to console
                if (message.find("[ERROR]") != std::string::npos)
                {
                    std::cerr << message << std::endl;
                }
                else
                {
                    std::cout << message << std::endl;
                }

                // log to file
                if (saveToFile && logFile.is_open())
                {
                    logFile << message << std::endl;
                }
            }
        }
    }
}