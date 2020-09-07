#include "reader.hh"

#include <cctype>
#include <stdexcept>

namespace lispc {

static bool issymbol(int c) {
  return isalnum(c) || std::string("+-*/%^!?").find(c) != std::string::npos;
}

static std::shared_ptr<Expression> make_expr_of_type(ExpressionType type) {
  auto expr = std::make_shared<Expression>();
  expr->type = type;
  return expr;
}

Reader::Reader(std::string source) : source(std::move(source)), source_pos(0) {}

void Reader::skip_whitespace() {
  while (source_pos < source.size() && isspace(source[source_pos])) {
    source_pos++;
  }
}

std::shared_ptr<Expression> Reader::next() {
  skip_whitespace();
  if (source_pos >= source.size()) {
    throw std::runtime_error("unexpected end of input when calling next");
  }
  if (isdigit(source[source_pos])) {
    int n = 0;
    while (isdigit(source[source_pos])) {
      n = n * 10 + source[source_pos] - '0';
      source_pos++;
    }
    if (issymbol(source[source_pos])) {
      throw std::runtime_error("unexpected symbol " +
                               std::string(1, source[source_pos]));
    }
    auto expr = make_expr_of_type(ExpressionType::Integer);
    expr->integer = n;
    return expr;
  }
  if (issymbol(source[source_pos])) {
    auto begin = source_pos;
    while (issymbol(source[source_pos])) {
      source_pos++;
    }
    auto expr = make_expr_of_type(ExpressionType::Symbol);
    expr->symbol = source.substr(begin, source_pos - begin);
    return expr;
  }
  if (source[source_pos] == '(') {
    // Skip the opening bracket.
    source_pos++;
    skip_whitespace();
    auto head = make_expr_of_type(ExpressionType::Cons);
    head->cdr = make_expr_of_type(ExpressionType::Null);
    auto curr = head;
    while (source_pos < source.size() && source[source_pos] != ')') {
      auto next_cell = make_expr_of_type(ExpressionType::Cons);
      next_cell->car = next();
      curr->cdr = next_cell;
      curr = next_cell;
      skip_whitespace();
    }
    if (source_pos >= source.size()) {
      throw std::runtime_error("unexpected end of input when reading list");
    }
    // Skip the closing bracket.
    source_pos++;
    curr->cdr = make_expr_of_type(ExpressionType::Null);
    return head->cdr;
  }
  throw std::runtime_error("unexpected symbol [" +
                           std::string(1, source[source_pos]) + "]");
}

bool Reader::done() {
  skip_whitespace();
  return source_pos >= source.size();
}

};  // namespace lispc