#include <memory>
#include <string>

#include "expr.hh"

namespace lispc {

class Reader {
 public:
  Reader(std::string source);

  std::shared_ptr<Expression> next();

  bool done();

 private:
  void skip_whitespace();

  std::string source;
  std::string::size_type source_pos;
};

};  // namespace lispc