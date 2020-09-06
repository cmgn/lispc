#include <memory>
#include <string>

namespace lispc {

enum ExpressionType {
  Cons,
  Integer,
  Symbol,
  Null,
};

struct Expression {
  ExpressionType type;

  // Cons fields.
  struct std::shared_ptr<Expression> car;
  struct std::shared_ptr<Expression> cdr;

  // Symbol fields.
  std::string symbol;

  // Integer fields.
  long long integer;

  friend std::ostream& operator<<(std::ostream& out, const Expression& expr);
  friend std::ostream& operator<<(std::ostream& out, const Expression* expr);
};

};  // namespace lispc