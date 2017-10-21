#include "json/json.hpp"
#include <string>
using json = nlohmann::json;

#include "LSProtocol.h"

void InitializeParams::Parse(json::object_t *Params) {

  for (auto &Param : *Params) {
    auto Key = Param.first;
    auto Value = Param.second;

    if (Key == "processId") {
      auto Value = Param.second;
      if (Value.is_number()) {
        ProcessId = Value;
      } else if (Value.is_null()) {
        ProcessId = 0;
      }
    }

    if (Key == "documentUri") {
      if (Value.is_string()) {
        DocumentUri = Value;
      } else if (Value.is_null()) {
        DocumentUri = "";
      } else {
      }
    } else if (Key == "rootPath") {
      if (Value.is_string()) {
        DocumentUri = Value;
      } else if (Value.is_null()) {
        DocumentUri = "";
      } else {
      }
    }

    if (Key == "trace") {
      if (Value.is_string()) {
        if (Value == "off")
          TraceLevel = TraceLevels::Off;
        if (Value == "messages")
          TraceLevel = TraceLevels::Messages;
        if (Value == "verbose")
          TraceLevel = TraceLevels::Verbose;
      } else {
      }
    }
  }
}