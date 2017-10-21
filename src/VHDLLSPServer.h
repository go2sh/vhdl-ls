#ifndef VHDLLSPSERVER_H
#define VHDLLSPSERVER_H

#include <istream>

#include "LSProtocol.h"
#include "JSONRPCDispatcher.h"

class LSPCallbacks {

public:
    virtual void onInitialize(RequestContext Context, InitializeParams &Params) = 0;
    virtual void onHover(RequestContext Context, TextDocumentPositionParams &Params) = 0;
    virtual void onDefinition(RequestContext Context, TextDocumentPositionParams &Params) = 0;
};

class VHDLLSPServer : public LSPCallbacks{

public:
    VHDLLSPServer(JSONOutput &Out) : Out(Out), Dispatcher() {    };

    void runLSPServer(std::istream &In);
    virtual void onInitialize(RequestContext Context, InitializeParams &Params);
    virtual void onHover(RequestContext Context, TextDocumentPositionParams &Params);
    virtual void onDefinition(RequestContext Context, TextDocumentPositionParams &Params);

private:
    JSONOutput &Out;
    JSONRPCDispatcher Dispatcher;

    void runLSPServerLoop(std::istream &In);
    void RegisterCallbacks();
    template <typename Param>
    void RegisterCallback(const std::string &Method, void (LSPCallbacks::*Handler)(RequestContext, Param));

};

#endif