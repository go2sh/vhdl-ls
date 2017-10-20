#include <iostream>
#include <fstream>

#include "JSONRPCDispatcher.h"
#include "VHDLLSPServer.h"

int main() {
  std::ofstream myfile ("C:\\example.txt");
  VHDLLSPServer Server(JSONOutput(std::cout, myfile));

  Server.runLSPServer(std::cin);

  myfile.close();
  return 0;
}