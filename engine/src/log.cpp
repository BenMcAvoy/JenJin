#include "jenjin/log.h"
#include <fmt/format.h>

using namespace Jenjin;

void handle(std::unordered_map<std::string, int> &map,
            const std::string &data) {
  bool exists = map.find(data) != map.end();

  if (!exists)
    map[data] = 1;

  if (exists)
    map[data]++;
}

void LogSink::log(const spdlog::details::log_msg &msg) {
  std::lock_guard<std::mutex> lock(mutex);
  colour_sink->log(msg);

  std::string data = msg.payload.data();
  bool exists = false;
  switch (msg.level) {
  case spdlog::level::trace:
    handle(trace_logs, data);
    break;
  case spdlog::level::debug:
    handle(debug_logs, data);
    break;
  case spdlog::level::info:
    handle(info_logs, data);
    break;
  case spdlog::level::warn:
    handle(warn_logs, data);
    break;
  case spdlog::level::err:
    handle(error_logs, data);
    break;
  case spdlog::level::critical:
    handle(critical_logs, data);
    break;
  case spdlog::level::off:
    break;
  default:
    break;
  }
}
