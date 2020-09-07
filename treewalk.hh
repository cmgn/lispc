#ifndef _TREEWALK_HH
#define _TREEWALK_HH

#include "expr.hh"

namespace lispc {

std::shared_ptr<Expression> evaluate(std::shared_ptr<Expression> expr,
                                     std::shared_ptr<Environment> env);

std::shared_ptr<Environment> global_scope();

};  // namespace lispc

#endif