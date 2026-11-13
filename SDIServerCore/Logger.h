#pragma once
#include <mutex>
#include <string>
#include <queue>
#include <fstream>
#include <condition_variable>

enum class LogLevel { INFO, WARN, ERR };

enum
{
    ServerStatsPanelLines = 13,
    ProfilerPanelLines = 5,

    ServerStatsStartLine = 0,
    ProfilerStartLine = ServerStatsStartLine + ServerStatsPanelLines,

    LoggerLine = ProfilerStartLine + ProfilerPanelLines,
};

///////////////////////////////////////////////////////
//	<	Logger	>
///////////////////////////////////////////////////////
class Logger
{
public:
    Logger() = default;
    ~Logger() { Shutdown(); }

    void Init(const wstring& filename);
    void Write(LogLevel level, const wstring& msg);
    void Shutdown();

    static Mutex& GetConsoleLock() { return _consoleLock; }

private:
    void LogThreadMain();
    wstring Timestamp();
    wstring LevelToString(LogLevel level);

private:
    struct LogEntry {
        LogLevel level;
        wstring message;
        uint32 threadId;
    };

    static Mutex           _consoleLock;
    Mutex                  _queueLock;
    std::wofstream         _file;
    bool                   _running = false;

    CondVar                _cv;
    Queue<LogEntry>        _logQueue;
};

#define LOG_INFO(msg)   do { if (GLogger) GLogger->Write(LogLevel::INFO, msg); } while(0)
#define LOG_WARN(msg)   do { if (GLogger) GLogger->Write(LogLevel::WARN, msg); } while(0)
#define LOG_ERROR(msg)  do { if (GLogger) GLogger->Write(LogLevel::ERR,  msg); } while(0)