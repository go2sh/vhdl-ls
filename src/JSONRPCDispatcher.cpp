#include "json/json.hpp"

#include "JSONRPCDispatcher.h"

using json = nlohmann::json;

void JSONRPCDispatcher::RegisterHandler(std::string Method, Handler H) {
    Handlers[Method] = std::move(H);
}

void JSONRPCDispatcher::Call(std::string &Message, JSONOutput &Out) {
  json Dispatch;

  try {
    Dispatch = json::parse(Message);
  } catch (std::exception &e) {
    return;
  }

  if (!Dispatch.is_object()) {
      Out.logMessage("Incomming message is not a JSON object.");
      return;
  }

  json Id;
  std::string Method;
  json Params;

  for (auto &Element : json::iterator_wrapper(Dispatch)) {
      std::string Key = Element.key();
      json Value = Element.value();

      if (Key == "jsonrpc") {
          if (!Value.is_string() && Value != "2.0") return;
      }
      if (Key == "id") {
          Id = Value;
      }

      if (Key == "method") {
          if (Value.is_string()) {
              Method = Value;
          } else {
              return;
          }
      }
      if (Key == "params") {
          Params = Value;
      }
  }

  CallHandler(Method, Params.get_ptr<json::object_t*>(), Id, Out);
}

void JSONRPCDispatcher::CallHandler(std::string Method, json::object_t *Params, json &Id, JSONOutput &Out) {
    if (Handlers.find(Method) == Handlers.end())
        return;
    Handlers[Method](RequestContext(Out, Id), Params);
}