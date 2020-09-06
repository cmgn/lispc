#include "reader.hh"

#include <fstream>
#include <iostream>
#include <sstream>

std::string read_file(const std::string path) {
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  return buf.str();
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    lispc::Reader rdr(read_file(std::string(argv[i])));
    while (!rdr.done()) {
      std::cout << rdr.next() << "\n";
    }
  }
}