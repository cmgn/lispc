#include <fstream>
#include <iostream>
#include <sstream>

#include "reader.hh"
#include "treewalk.hh"

std::string read_file(const std::string path) {
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  return buf.str();
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    lispc::Reader rdr(read_file(argv[i]));
    while (!rdr.done()) {
      auto scope = lispc::global_scope();
      auto expr = rdr.next();
      std::cout << lispc::evaluate(expr, scope) << "\n";
    }
  }
}