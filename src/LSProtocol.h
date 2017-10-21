#ifndef LSPROTOCOL_H
#define LSPROTOCOL_H
#include <string>

#include "json/json.hpp"

using json = nlohmann::json;

class Params {
public:
    virtual void Parse(json::object_t *params) = 0;
};

enum class TraceLevels {
    Off,
    Messages,
    Verbose
};


class InitializeParams : public Params {

public:
    uint32_t ProcessId;
    std::string DocumentUri = "";
    TraceLevels TraceLevel;

    virtual void Parse(json::object_t *Params);

};
#endif // !LSPROTOCOL_H