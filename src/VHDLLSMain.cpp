#include <iostream>
#include <fstream>

#include "JSONRPCDispatcher.h"
#include "VHDLLSPServer.h"

int main() {
  std::ofstream myfile ("/home/chris/example.txt");
  JSONOutput Out(std::cout, std::cerr);
  VHDLLSPServer Server(Out);

  Server.runLSPServer(std::cin);

  myfile.close();
  return 0;
}