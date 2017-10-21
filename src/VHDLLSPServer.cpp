#include <iostream>
#include <istream>
#include <string>

#include "json/json.hpp"
#include "VHDLLSPServer.h"

using json = nlohmann::json;

template <typename Param>
void VHDLLSPServer::RegisterCallback(
    const std::string &Method,
    void (LSPCallbacks::*Handler)(RequestContext, Param)) {
  Dispatcher.RegisterHandler(
      Method, [=](RequestContext Context, json::object_t *RawParams) {
        typename std::decay<Param>::type P;
        P.Parse(RawParams);
        (this->*Handler)(std::move(Context), P);
      });
}

void VHDLLSPServer::onInitialize(RequestContext Context,
                                 InitializeParams &Params) {
  json h;
  json x;
  x["hoverProvider"] = true;
  x["definitionProvider"] = true;
  x["textDocumentSync"] = 0;
  h["capabilities"] = x;
  Context.Reply(h);
}

void VHDLLSPServer::onHover(RequestContext Context, TextDocumentPositionParams &Params) {
  Hover H;
  H.Contents.push_back(MarkedString("Test 23"));
  
  Context.Reply(H.dump());
}

void VHDLLSPServer::onDefinition(RequestContext Context, TextDocumentPositionParams &Params) {
  Range R;
  R.Start.Line = 0;
  R.Start.Character = 0;
  R.End.Line = 0;
  R.End.Character = 1;
  Location L(Params.DocumentUri, R);

  Context.Reply(L.dump());
}

void VHDLLSPServer::runLSPServer(std::istream &In) {
  RegisterCallbacks();

  runLSPServerLoop(In);
}

void VHDLLSPServer::runLSPServerLoop(std::istream &In) {
  std::string LengthHeader = "Content-Length: ";
  while (1) {
    uint32_t Bytes = 0;
    std::string Line;

    while (1) {
      try {
        std::getline(In, Line);
      } catch (std::exception &e) {
        return;
      }
      if (Line.compare(0, LengthHeader.length(), LengthHeader) == 0) {
        Bytes = stoul(Line.substr(LengthHeader.length()));
      } else if (Line.length() == 0 && Bytes > 0) {
        break;
      } else {
        continue;
      }
    }

    std::string Data(Bytes, '\0');
    std::cin.read(&Data[0], Bytes);

    Dispatcher.Call(Data, Out);
  }
}

void VHDLLSPServer::RegisterCallbacks() {
  RegisterCallback("initialize", &LSPCallbacks::onInitialize);
  RegisterCallback("textDocument/hover", &LSPCallbacks::onHover);
  RegisterCallback("textDocument/definition", &LSPCallbacks::onDefinition);
}
