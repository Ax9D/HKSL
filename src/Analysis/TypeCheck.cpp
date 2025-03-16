#include "AST.h"
#include "Util.h"
#include "Visitor.h"
#include <Analysis/TypeCheck.h>
#include <cassert>
#include <format>
#include <unordered_map>
namespace HKSL {
Type* TypeInferenceVisitor::type_of(const Expr *expr) {
  auto ty = type_of_expr(expr);
  return ty;
}
Type* TypeInferenceVisitor::type_of_expr(const Expr *expr) {
  switch (expr->kind()) {
  case ExprKind::BinExpr:
    return type_of_binary_expr((const BinExpr *)expr);
  case ExprKind::UnaryExpr:
    return type_of_unary_expr((const UnaryExpr *)expr);
  case ExprKind::NumberConstant:
    return type_of_number_constant((const NumberConstant *)expr);
  case ExprKind::Variable:
    return type_of_variable((const Variable *)expr);
  case ExprKind::VarDecl:
    return type_of_var_decl((const VarDecl *)expr);
  case ExprKind::CallExpr:
    return type_of_call_expr((const CallExpr *)expr);
  case ExprKind::AssignmentExpr:
    return type_of_assignment_expr((const AssignmentExpr *)expr);
  case ExprKind::LetExpr:
    return type_of_let_expr((const LetExpr *)expr);
  }

  HKSL_UNREACHABLE();
}
Type* TypeInferenceVisitor::type_of_variable(const Variable *var) {
  auto var_decl = context.symbol_resolver().get_var_decl(var);

  if(!var_decl->type) {
    return nullptr;
  }

  auto type = *var_decl->type;

  return type;
}
Type* TypeInferenceVisitor::type_of_let_expr(const LetExpr *expr) {
  return context.type_registry().get_void();
}
Type* TypeInferenceVisitor::type_of_assignment_expr(const AssignmentExpr *expr) {
  Type* type_left = type_of_expr(expr->lhs.get());
  Type* type_right = type_of_expr(expr->rhs.get());

  if (!type_left) {
    return nullptr;
  }
  if (!type_right) {
    return nullptr;
  }

  if (type_left != type_right) {
    context.error(expr->eq_token.span, std::format("Types of left and right side of assignment don't match, "
                      "left: {}, right: {}",
                      type_left->name(), type_right->name()));
    return nullptr;
  }

  return type_left;
}
Type* TypeInferenceVisitor::type_of_call_expr(const CallExpr *expr) { 
  auto function = context.symbol_resolver().get_function(expr);
  auto return_type = function->m_return_type;

  if(function->m_args.size() != expr->args.size()) {
    context.error(function->m_name.span, std::format("Incorrect no. of args of function call, provided {}, expected {}", expr->args.size(), function->m_args.size()));
  }
  for(size_t i = 0; i < function->m_args.size(); i++) {
    auto parameter_type = *function->m_args[i].type;
    auto arg_type = type_of_expr(expr->args[i].get());
    if(!arg_type) {
      break;
    }
    if(parameter_type != arg_type) {
      context.error(expr->fn_name.span, std::format("Expected function argument at index {} to be: {}, got: {}", i, arg_type->name(), parameter_type->name()));
    }
  }

  return return_type;
}
Type* TypeInferenceVisitor::type_of_var_decl(const VarDecl *decl) {
  return context.type_registry().get_void();
}
Type* TypeInferenceVisitor::type_of_unary_expr(const UnaryExpr *expr) {
  assert(expr->op == UnaryOp::Negate);

  Type* type_inner = type_of_expr(expr->expr.get());

  if (!type_inner) {
    return nullptr;
  }

  if (type_inner->kind() == TypeKind::Void) {
    context.error(expr->op_token.span, "Cannot negate type void");
    return nullptr;
  }

  return type_inner;
}
Type* TypeInferenceVisitor::type_of_binary_expr(const BinExpr *expr) {
  Type* type_left = type_of_expr(expr->left.get());
  Type* type_right = type_of_expr(expr->right.get());

  if (!type_left) {
    return nullptr;
  }
  if (!type_right) {
    return nullptr;
  }

  if (type_left != type_right) {
    auto message = std::format("Types of left and right side of binary "
                               "expression don't match, left: {}, right: {}",
                               type_left->name(), type_right->name());
    context.error(expr->op_token.span, message);
    return nullptr;
  }

  return type_left;
}

Type* TypeInferenceVisitor::type_of_number_constant(const NumberConstant *expr) {
  return context.type_registry().get_float();
}

TypeInferenceVisitor::TypeInferenceVisitor(CompilationContext& _context): context(_context) {}

void TypeInferenceVisitor::visit_function(Function* function) {
  outer_fn = function;
  Visitor::visit_function(function);
  outer_fn = std::nullopt;
}
void TypeInferenceVisitor::visit_return_statement(ReturnStatement* ret) {
  assert(outer_fn);
  auto type_ret = type_of(ret->value->get());
  if(!type_ret) {
    return;
  }
  auto fn_ret_type = (*outer_fn)->m_return_type;
  Type* type_fn_ret;
  if(!fn_ret_type) {
    type_fn_ret = context.type_registry().get_void();
  } else {
    type_fn_ret = fn_ret_type;
  }

  if(type_ret != type_fn_ret) {
    context.error(ret->ret_token.span, std::format("Incorrect return type, expected: {}, got: {}", type_fn_ret->name(), type_ret->name()));
  }
}

void TypeInferenceVisitor::visit_expr(Expr* expr) {
  Visitor::visit_expr(expr);

  auto type = type_of_expr(expr);

  if(type) {
    context.type_resolver().register_expr(expr, type);
  }

}
void TypeInferenceVisitor::visit_let_expr(LetExpr* expr) {
    std::optional<Type*> type_left = expr->var_decl->type;
    std::optional<Type*> type_right = std::nullopt;

    if(expr->rhs) {
      type_right = type_of(expr->rhs->get());
    }

    if(!type_left && !type_right) {
        // 1. There is on type explicitly provided AND
        // 2. We couldn't infer type on the right :(
        context.error(expr->var_decl->name.span, std::format("Couldn't infer type for variable {}, please specify its manually", expr->var_decl->name.name));
        return;
    }

    if(!type_left && type_right) {
      type_left = type_right;
      expr->var_decl->type =  type_right;
      return;
    }

    if(type_left && type_right) {
      if(type_left != type_right) {
        context.error(expr->var_decl->name.span, std::format("Type of left and right hand side of variable declation don't match, left: {}, right: {}", (*type_left)->name(), (*type_right)->name()));
      }
    }

    if(expr->rhs) {
      Visitor::visit_expr(expr->rhs->get());
    }
}
bool TypeInferenceVisitor::run() {
  // TypeExistsVisitor type_exists(context);
  // if(!type_exists.check_all_types_exists()) {
  //   return false;
  // }
  visit(context.get_ast());

  return context.is_success();
}
}