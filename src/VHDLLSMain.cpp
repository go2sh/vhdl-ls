#include <lsp/LanguageServer.h>
#include <lsp/LanguageClient.h>
#include <lsp/jsonrpc/MessageConnection.h>
#include <lsp/LSPLogger.h>
#include <lsp/lsp.h>


#include "Diag/DiagnosticConsumer.h"
#include "Diag/DiagnosticEngine.h"
#include "Common/SourceManager.h"
#include "Common/TokenKinds.h"
#include "Parse/Lexer.h"
#include "Parse/Parser.h"

#include <cstdlib>
#include <asio.hpp>

using namespace vc;

class LspConsumer : public DiagnosticConsumer {
    SourceManager Mgr;
    lsp::LanguageClient *Client;
public:
    std::map<std::string,lsp::PublishDiagnosticsParams> FileMap;

    LspConsumer(const SourceManager &Mgr, lsp::LanguageClient *Client) : Mgr(Mgr), Client(Client) {}
    virtual void handleDiagnostic(const Diagnostic & Diag) {
      auto data = Mgr.getDecomposedLocation(Diag.getLocation());
      uint32_t Line = Mgr.getLineNumber(data.first, data.second) - 1;
      uint32_t Column = Mgr.getColumnNumber(data.first, data.second);
      std::string FileUri = "file://"+Mgr.getPath(data.first);
      
      auto DiagPElement = FileMap.find(FileUri);
      if (DiagPElement == FileMap.end()) {
        lsp::PublishDiagnosticsParams DiagP;
        DiagP.Uri = FileUri;
        DiagPElement = FileMap.insert(std::pair(FileUri, DiagP)).first;
      }
       
      lsp::Diagnostic D;
      D.Severity = lsp::DiagnosticSeverity::Error;
      D.Message = Diag.getString();
      D.Source = "vhdl-ls";
      D.Range.Start = {Line,Column};
      D.Range.End = {Line, Column + 1};
      DiagPElement->second.Diagnostics.push_back(D);
    }
};

class TestServer : public lsp::LanguageServer {
  lsp::LanguageClient *Client;
public:
  TestServer(lsp::LanguageClient *Client = nullptr) : Client(Client) {
  }
  virtual lsp::InitializeResult
  initialize(const lsp::InitializeParams<lsp::EmptyInitializationOptions> & Params) {
    lsp::InitializeResult Result;
    Result.Capabilities.HoverProvider = true;
    Result.Capabilities.CompletionProvider = {true, {".","'"}};
    Result.Capabilities.TextDocumentSync = { true, lsp::TextDocumentSyncKind::Full, false, false, {}};
    return Result;
  }

  virtual void shutdown() { 
  
  }

  virtual void exit() {
    std::exit(0);
  }

  virtual lsp::Hover hover(const lsp::TextDocumentPositionParams & Params) {
    lsp::Hover H;
    H.Contents.Language = "vhdl";
    H.Contents.Value = "Works";
    return H;
  }

  virtual std::vector<lsp::CompletionItem<lsp::EmptyCompletionData>> completion(const lsp::CompletionParams &Params) {
    lsp::CompletionItem<lsp::EmptyCompletionData> I;
    std::vector<lsp::CompletionItem<lsp::EmptyCompletionData>> Result;
    I.Label = "hase hase";
    Result.push_back(I);
    return Result; 
  }

  virtual void textDocumentDidChange(const lsp::TextDocumentDidChangeParams &Params) {
    SourceManager SrcMgr;
    SourceFile File = SrcMgr.createSourceFile(Params.TextDocument.Uri.substr(7));
    SourceLocation Loc = SourceLocation::fromRawEncoding(1);
    DiagnosticEngine Engine;
    LspConsumer Consumer(SrcMgr,Client);
    Engine.addConsumer(&Consumer);
    Lexer lexer(Engine, Loc, SrcMgr.getBuffer(File));
    Parser P(Engine, &lexer);
    P.parseDesignFile();
    
    for (auto Ele : Consumer.FileMap) {
      Client->publishDiagnostics(Ele.second);
    }
  }
};

int main(int argc, char *argv[]) {
  asio::io_service io_service;
  lsp::MessageConnection *Connection = lsp::createAsioTCPConnection(io_service, std::string(argv[1]), std::string(argv[2]));
  lsp::ProxyLanguageClient Client(*Connection);
  TestServer Server(&Client);
  Server.connect(*Connection);
  lsp::LspLogger *Log = new lsp::LspLogger(&Client);
  Connection->setLogger(Log);
  Connection->listen();

  std::thread ConnectionThread =
      std::thread([&io_service] { io_service.run(); });
  while (true) {
    Connection->processMessageQueue();
  }

  ConnectionThread.join();
}