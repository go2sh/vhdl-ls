#ifndef JSONRPCDISPATCHER_H
#define JSONRPCDISPATCHER_H

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include "json/json.hpp"
using json = nlohmann::json;

class JSONOutput {

public:
  JSONOutput(std::ostream &Out, std::ostream &Log) : Out(Out), Log(Log){};

  void writeMessage(const std::string &Message) {
    Log << "-->\n" << Message << "\n" << std::flush;
    Log.flush();

    Out << "Content-Length: " << Message.length() << "\r\n\r\n" << Message << std::flush;
    Out.flush();
  }

  void logMessage(const std::string &Message) {
    Log << Message;
    Log.flush();
  }

private:
  std::ostream &Out;
  std::ostream &Log;
};

class RequestContext {
public:
  RequestContext(JSONOutput &Out, json Id) : Out(Out), Id(Id){};

  void Reply(const json &Result) {
    json Message;
    Message["jsonrpc"] = "2.0";
    Message["id"] = Id;
    Message["result"] = Result;
    Out.writeMessage(Message.dump());
  };

private:
  JSONOutput Out;
  json Id;
};

class JSONRPCDispatcher {
public:
  JSONRPCDispatcher(){};

  using Handler = std::function<void(RequestContext, json::object_t*)>;

  void RegisterHandler(std::string Method, Handler H);
  void Call(std::string &Message, JSONOutput &Out);

private:
  std::map<std::string, Handler> Handlers;

  void CallHandler(std::string Method, json::object_t *Params, json &Id, JSONOutput &Out);
};

#endif // !JSONRPCDISPATCHER_H