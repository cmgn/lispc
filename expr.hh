#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace lispc {

enum ExpressionType {
  Cons,
  Integer,
  Symbol,
  Null,
  Builtin,
  Closure,
};

struct Environment;

struct Expression {
  ExpressionType type;

  // Cons fields.
  struct std::shared_ptr<Expression> car;
  struct std::shared_ptr<Expression> cdr;

  // Symbol fields.
  std::string symbol;

  // Integer fields.
  long long integer;

  // Builtin fields.
  std::string builtin_name;
  std::function<std::shared_ptr<Expression>(
      std::shared_ptr<Expression> args,
      std::shared_ptr<Environment> environment)>
      builtin;

  // Closure fields.
  std::shared_ptr<Expression> closure_expr;
  std::shared_ptr<Environment> closure_environment;

  friend std::ostream& operator<<(std::ostream& out, const Expression& expr);
  friend std::ostream& operator<<(std::ostream& out, const Expression* expr);
};

struct Environment {
  Environment* parent;
  std::unordered_map<std::string, std::shared_ptr<Expression>> variables;

  std::shared_ptr<Expression> lookup(const std::string& name);
};

};  // namespace lispc