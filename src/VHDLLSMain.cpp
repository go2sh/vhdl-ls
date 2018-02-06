#include <LanguageServer.h>
#include <LanguageClient.h>
#include <MessageConnection.h>
#include <ServerConnection.h>

#include "Diag/DiagnosticConsumer.h"
#include "Diag/DiagnosticEngine.h"
#include "Common/SourceManager.h"
#include "Common/TokenKinds.h"
#include "Parse/Lexer.h"
#include "Parse/Parser.h"

#include <cstdlib>

using namespace vc;

class LspConsumer : public DiagnosticConsumer {
    SourceManager Mgr;
    lsp::LanguageClient *Client;
public:
    LspConsumer(const SourceManager &Mgr, lsp::LanguageClient *Client) : Mgr(Mgr), Client(Client) {}
    virtual void handleDiagnostic(const Diagnostic & Diag) {
      auto data = Mgr.getDecomposedLocation(Diag.getLocation());
      uint32_t Line = Mgr.getLineNumber(data.first, data.second) - 1;
      uint32_t Column = Mgr.getColumnNumber(data.first, data.second);
      lsp::PublishDiagnosticsParams DiagP;
      DiagP.Uri = "file://"+Mgr.getPath(data.first);
      lsp::Diagnostic D;
      D.Severity = lsp::DiagnosticSeverity::Error;
      D.Message = Diag.getString();
      D.Source = "vhdl-ls";
      D.Range.Start = {Line,Column};
      D.Range.End = {Line, Column + 1};
      DiagP.Diagnostics.push_back(D);
      Client->publishDiagnostics(DiagP);
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
  }
};

int main(int argc, char **argv) {
  lsp::StdinMessageReader Reader;
  lsp::StdoutMessageWriter Writer;
  lsp::StdIoLogger Log;
  lsp::MessageConnection Connection(&Reader, &Writer, &Log);
  lsp::ProxyLanguageClient Client(Connection);
  TestServer Server(&Client);
  Server.connect(Connection);

  std::thread ConnectionThread =
      std::thread([&Connection] { Connection.run(); });
  while (true) {
    Connection.processMessageQueue();
  }

  ConnectionThread.join();
}