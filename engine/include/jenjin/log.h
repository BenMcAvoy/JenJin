#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace Jenjin {
// A logger that writes to stdout and to a std::vector internally for display in
// ImGui
class LogSink : public spdlog::sinks::sink {
public:
  LogSink() {
    colour_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    colour_sink->set_level(spdlog::level::trace);
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::set_pattern("%v");
  }

  void log(const spdlog::details::log_msg &msg) override;
  void flush() override {}
  void set_pattern(const std::string &pattern) override {}
  void
  set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {}

  std::unordered_map<std::string, int> &GetTraceLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    return trace_logs;
  }

  std::unordered_map<std::string, int> &GetDebugLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    return debug_logs;
  }

  std::unordered_map<std::string, int> &GetInfoLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    return info_logs;
  }

  std::unordered_map<std::string, int> &GetWarnLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    return warn_logs;
  }

  std::unordered_map<std::string, int> &GetErrorLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    return error_logs;
  }

  std::unordered_map<std::string, int> &GetCriticalLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    return critical_logs;
  }

  void ClearLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    trace_logs.clear();
    debug_logs.clear();
    info_logs.clear();
    warn_logs.clear();
    error_logs.clear();
    critical_logs.clear();
  }

  ~LogSink() override {}

private:
  // map: Line number -> details, times printed

  std::unordered_map<std::string, int> trace_logs;
  std::unordered_map<std::string, int> debug_logs;
  std::unordered_map<std::string, int> info_logs;
  std::unordered_map<std::string, int> warn_logs;
  std::unordered_map<std::string, int> error_logs;
  std::unordered_map<std::string, int> critical_logs;

  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> colour_sink;

  std::mutex mutex;
};
} // namespace Jenjin
