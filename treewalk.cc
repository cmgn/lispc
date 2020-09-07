#include "treewalk.hh"

#include <sstream>
#include <stdexcept>
#include <string>

namespace lispc {

static std::shared_ptr<Environment> create_subscope(std::shared_ptr<Environment> env) {
  auto new_env = std::make_shared<Environment>();
  new_env->parent = env;
  return new_env;
}

static std::string expr_to_string(std::shared_ptr<Expression> expr) {
  std::stringstream ss;
  ss << expr;
  return ss.str();
}

static std::vector<std::shared_ptr<Expression>> cons_chain_to_vector(
    std::shared_ptr<Expression> expr) {
  if (expr->type != ExpressionType::Cons) {
    throw std::runtime_error("cannot convert non-cons cell to vector");
  }
  std::vector<std::shared_ptr<Expression>> exprs;
  auto head = expr;
  while (head->type == ExpressionType::Cons) {
    exprs.push_back(head->car);
    head = head->cdr;
  }
  if (head->type != ExpressionType::Null) {
    throw std::runtime_error("cons lists must be terminated with null; got " + expr_to_string(head));
  }
  return exprs;
}

static std::vector<std::shared_ptr<Expression>> cons_chain_to_evaluated_vector(
    std::shared_ptr<Expression> expr, std::shared_ptr<Environment> env) {
  auto unevaluated = cons_chain_to_vector(expr);
  for (decltype(unevaluated)::size_type i = 0; i < unevaluated.size(); i++) {
    unevaluated[i] = evaluate(unevaluated[i], env);
  }
  return unevaluated;
}


std::shared_ptr<Expression> evaluate(std::shared_ptr<Expression> expr,
                                     std::shared_ptr<Environment> env) {
  if (expr->type == ExpressionType::Integer ||
      expr->type == ExpressionType::Builtin ||
      expr->type == ExpressionType::Closure ||
      expr->type == ExpressionType::Null) {
    return expr;
  } else if (expr->type == ExpressionType::Symbol) {
    return env->lookup(expr->symbol);
  } else if (expr->type != ExpressionType::Cons) {
    throw std::runtime_error("unknown expression type passed to evaluate");
  }
  // Must be a list.
  auto func = evaluate(expr->car, env);
  if (func->type == ExpressionType::Builtin) {
    return func->builtin(expr->cdr, create_subscope(env));
  } else if (func->type == ExpressionType::Closure) {
    // Evaluate the arguments and bind them.
    auto arguments = cons_chain_to_evaluated_vector(expr->cdr, env);
    if (arguments.size() != func->closure_parameters.size()) {
      throw std::runtime_error("argument count does not match parameter count");
    }
    auto new_env = create_subscope(func->closure_environment);
    for (decltype(arguments)::size_type i = 0; i < arguments.size(); i++) {
      new_env->variables[func->closure_parameters[i]] = arguments[i];
    }
    return evaluate(func->closure_expr, new_env);
  } else {
    throw std::runtime_error("cannot call expression " + expr_to_string(func));
  }
}

static std::shared_ptr<Expression> make_builtin(
    const std::string& name, decltype(Expression::builtin) builtin) {
  auto expr = std::make_shared<Expression>();
  expr->type = ExpressionType::Builtin;
  expr->builtin_name = name;
  expr->builtin = builtin;
  return expr;
}

std::shared_ptr<Environment> global_scope() {
  auto global_environment = std::make_shared<Environment>();
  auto lambda = make_builtin(
      "lambda",
      [&](std::shared_ptr<Expression> args,
          std::shared_ptr<Environment> env) -> std::shared_ptr<Expression> {
        auto args_vector = cons_chain_to_vector(args);
        if (args_vector.size() != 2) {
          throw std::runtime_error("lambda takes 2 arguments; got " +
                                   std::to_string(args_vector.size()));
        }
        // Get the paramter names.
        std::vector<std::string> param_names;
        auto param_list_head = args_vector[0];
        while (param_list_head->type == ExpressionType::Cons) {
          auto param = param_list_head->car;
          if (param_list_head->car->type != ExpressionType::Symbol) {
            throw std::runtime_error(
                "lambda param name must be a symbol; got " +
                expr_to_string(param));
          }
          param_names.push_back(param->symbol);
          param_list_head = param_list_head->cdr;
        }
        // Get the body.
        auto body = args_vector[1];
        auto closure = std::make_shared<Expression>();
        closure->type = ExpressionType::Closure;
        closure->closure_parameters = param_names;
        closure->closure_expr = body;
        closure->closure_environment = env;
        return closure;
      });
  global_environment->variables[lambda->builtin_name] = lambda;
  auto add = make_builtin(
      "+",
      [&](std::shared_ptr<Expression> args,
          std::shared_ptr<Environment> env) -> std::shared_ptr<Expression> {
        auto exprs = cons_chain_to_evaluated_vector(args, env);
        auto total = std::make_shared<Expression>();
        total->type = ExpressionType::Integer;
        total->integer = 0;
        for (const auto &expr : exprs) {
          if (expr->type != ExpressionType::Integer) {
            throw std::runtime_error("arguments to + must be integers");
          }
          total->integer += expr->integer;
        }
        return total;
      });
  global_environment->variables[add->builtin_name] = add;
  return global_environment;
}

};  // namespace lispc