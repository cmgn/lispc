#include "expr.hh"

#include <sstream>
#include <stdexcept>

namespace lispc {

std::ostream& operator<<(std::ostream& out, const Expression& expr) {
  out << &expr;
  return out;
}

std::ostream& operator<<(std::ostream& out, const Expression* expr) {
  switch (expr->type) {
    case ExpressionType::Integer:
      out << expr->integer;
      break;
    case ExpressionType::Symbol:
      out << expr->symbol;
      break;
    case ExpressionType::Null:
      out << "NULL";
      break;
    case ExpressionType::Cons: {
      out << "(";
      auto* curr = expr;
      while (curr->type == ExpressionType::Cons) {
        out << curr->car;
        curr = curr->cdr.get();
        if (curr->type != ExpressionType::Null) {
          out << " ";
        }
      }
      if (curr->type != ExpressionType::Null) {
        out << curr;
      }
      out << ")";
      break;
    }
    case ExpressionType::Builtin:
      out << "BUILTIN[" << expr->builtin_name << "]";
      break;
    case ExpressionType::Closure:
      out << "CLOSURE";
      break;
    default:
      throw std::runtime_error("unknown expr type");
  }
  return out;
}

std::shared_ptr<Expression> Environment::lookup(const std::string& name) {
  if (variables.count(name)) {
    return variables[name];
  }
  if (parent) {
    return parent->lookup(name);
  }
  throw std::runtime_error("variable " + name + " not found");
}

};  // namespace lispc