#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include <cdk/types/primitive_type.h>
// must come after other #includes
#include "og_parser.tab.h"

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void og::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void og::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void og::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}
void og::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
}
void og::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected inunary logical expression");
  node->type(node->argument()->type());
}
void og::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in binary expression");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in binary expression");

  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}
void og::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in binary expression");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in binary expression");

  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void og::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void og::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT) && !node->argument()->is_typed(cdk::TYPE_DOUBLE))
    throw std::string("wrong type in argument of unary expression");

  node->type(node->argument()->type());
}

void og::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void og::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)
  && !node->left()->is_typed(cdk::TYPE_DOUBLE)
  && !node->left()->is_typed(cdk::TYPE_POINTER))
    throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)
  && !node->right()->is_typed(cdk::TYPE_DOUBLE)
  && !node->right()->is_typed(cdk::TYPE_POINTER))
    throw std::string("wrong type in right argument of binary expression");

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_POINTER) || node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->left()->is_typed(cdk::TYPE_POINTER)
    ? node->type(node->left()->type())
    : node->type(node->right()->type());
  } 
  else
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void og::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void og::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw std::string("undeclared variable '" + id + "'");
  }
}

void og::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw std::string("undeclared variable '" + id + "'");
  }
}

void og::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  // TODO single node->type(node->lvalue()->type())

  node->lvalue()->accept(this, lvl + 4);
  _lvalueType = node->lvalue()->type();
  node->rvalue()->accept(this, lvl + 4);

  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(node->lvalue()->type());
    } else
      throw std::string("wrong assignment to integer");
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
    if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      auto lpointer = cdk::reference_type_cast(node->lvalue()->type());
      auto rpointer = cdk::reference_type_cast(node->rvalue()->type());

      if (lpointer->referenced() != rpointer->referenced()) {
        throw std::string("pointer reference types must match");
      }

      node->type(node->lvalue()->type());
    } else if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      //TODO: check that the integer is a literal and that it is nullptr
      node->type(cdk::make_primitive_type(4, cdk::TYPE_POINTER));
    } else
      throw std::string("wrong assignment to pointer");
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT) || node->rvalue()->is_typed(cdk::TYPE_DOUBLE)) {
      node->type(node->lvalue()->type());
    } else
      throw std::string("wrong assignment to real");
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {
    if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
      node->type(node->lvalue()->type());
    } else
      throw std::string("wrong assignment to string");
  }
  else {
    throw std::string("wrong types in assignment");
  }

  _lvalueType = nullptr;
}

//---------------------------------------------------------------------------

void og::type_checker::do_evaluation_node(og::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void og::type_checker::do_write_node(og::write_node *const node, int lvl) {
  _lvalueType = cdk::make_primitive_type(4, cdk::TYPE_INT);
  node->argument()->accept(this, lvl + 2);
  
  for (size_t i = 0; i < node->argument()->size(); i++) {
    cdk::expression_node * inode = (cdk::expression_node *) node->argument()->node(i);

    if (!(inode->is_typed(cdk::TYPE_INT)
    || inode->is_typed(cdk::TYPE_DOUBLE)
    || inode->is_typed(cdk::TYPE_STRING)))
      throw std::string("wrong type in write expression");
  }
  _lvalueType = nullptr;
}

//---------------------------------------------------------------------------

void og::type_checker::do_input_node(og::input_node *const node, int lvl) {
  node->type(_lvalueType);
}

//---------------------------------------------------------------------------

void og::type_checker::do_for_node(og::for_node *const node, int lvl) {
  node->inits()->accept(this, lvl + 4);
  node->condition()->accept(this, lvl + 4);
  node->incrs()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void og::type_checker::do_if_node(og::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void og::type_checker::do_if_else_node(og::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void og::type_checker::do_function_call_node(og::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  const std::string &id = node->identifier();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);

  if (symbol == nullptr) throw std::string("symbol '" + id + "' is undeclared.");
  if (!symbol->isFunction()) throw std::string("symbol '" + id + "' is not a function.");
  node->type(symbol->type());

  // Validate args against symbol
  cdk::sequence_node * arguments = node->arguments();
  cdk::sequence_node * func_arguments = symbol->arguments();

  if (arguments != nullptr && func_arguments != nullptr) {
    arguments->accept(this, lvl + 4);
    size_t size = arguments->size();

    if (size != func_arguments->size())
      throw std::string("no matching function for call to " + id + ".");

    for (size_t i = 0; i < size; i++) {
      cdk::expression_node * arg = (cdk::expression_node *) arguments->node(i);
      cdk::expression_node * func_arg = (cdk::expression_node *) func_arguments->node(i);

      if ((arg->type() != func_arg->type()) && !(arg->is_typed(cdk::TYPE_INT) && func_arg->is_typed(cdk::TYPE_DOUBLE))) {
        throw std::string("no matching function for call to " + id + ".");
      }
    }
  }

  // One of them has arguments
  else if (arguments != nullptr || func_arguments != nullptr)
    throw std::string("no matching function for call to " + id + ".");
}

void og::type_checker::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  std::string id;

  // "Fix" naming issues...
  if (node->identifier() == "og")
    id = "_main";
  else if (node->identifier() == "_main")
    id = "._main";
  else
    id = node->identifier();

  // Remember symbol so that args know
  std::shared_ptr<og::symbol> function = std::make_shared<og::symbol>(node->type(), id, true, node->qualifier(), node->arguments());

  std::shared_ptr<og::symbol> previous = _symtab.find(function->name());
  if (previous) {
    if ((previous->qualifier() == tPUBLIC && node->qualifier() == tPUBLIC)
    || (previous->qualifier() == tPRIVATE && node->qualifier() == tPRIVATE))  {
      _symtab.replace(function->name(), function);
      _parent->set_new_symbol(function);
    } else {
      throw std::string("conflicting definition for '" + function->name() + "'");
    }
  } else {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

void og::type_checker::do_function_definition_node(og::function_definition_node *const node, int lvl) {
  std::string id;

  // "Fix" naming issues...
  if (node->identifier() == "og")
    id = "_main";
  else if (node->identifier() == "_main")
    id = "._main";
  else
    id = node->identifier();

  std::shared_ptr<og::symbol> function = std::make_shared<og::symbol>(node->type(), id, true, node->qualifier(), node->arguments());

  std::shared_ptr<og::symbol> previous = _symtab.find(function->name());
  if (previous) {
    if ((previous->qualifier() == tPUBLIC && node->qualifier() == tPUBLIC)
    || (previous->qualifier() == tPRIVATE && node->qualifier() == tPRIVATE))  {
      _symtab.replace(function->name(), function);
      _parent->set_new_symbol(function);
    } else {
      throw std::string("conflicting definition for '" + function->name() + "'");
    }
  } else {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_return_node(og::return_node *const node, int lvl) {
  if (node->argument()) { node->argument()->accept(this, lvl + 2); }
}

void og::type_checker::do_break_node(og::break_node *const node, int lvl) {
  //TODO
}

void og::type_checker::do_continue_node(og::continue_node *const node, int lvl) {
  //TODO
}

//---------------------------------------------------------------------------

void og::type_checker::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {

  _lvalueType = node->type();
  
  size_t size;
  if (node->initializer() != nullptr) {
    node->initializer()->accept(this, lvl + 2);
    
    if (node->initializer()->is_typed(cdk::TYPE_STRUCT)) {
      auto structure = cdk::structured_type_cast(node->initializer()->type());
      size = structure->length();
    }
    else {
      size = 1;
    }
  } else {
    size = 0;
  }

  // OK: auto x = 1; || auto x, y, z = 1, 2, 3;
  // KO: auto x, y = 1, 2, 3;
  if (node->identifiers()->size() > 1 && size != node->identifiers()->size()) {
    throw std::string("wrong format for auto initializer.");
  }
  else if (size > 1) {
    // [ public ] auto identificadores = expressões

    std::vector<std::shared_ptr<cdk::basic_type>> components;

    if (node->identifiers()->size() == 1) {
      // auto i = 3, 61.0, "string1";
      
      auto id = *node->identifiers()->front();
      node->type(node->initializer()->type());
      
      std::shared_ptr<og::symbol> symbol = std::make_shared<og::symbol>(node->type(), id, false, node->qualifier(), nullptr);
      if (!_symtab.insert(id, symbol))
        throw std::string("variable '" + id + "' redeclared");
      _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
    } else {
      // auto i, j, k = 3, 61.0, "string1";
      
      auto structure = cdk::structured_type_cast(node->initializer()->type());
      node->type(node->initializer()->type());

      for (int i = structure->length() - 1; i >= 0; i--) {
        auto id = *node->identifiers()->at(i);

        std::shared_ptr<og::symbol> symbol = std::make_shared<og::symbol>(structure->component(i), id, false, node->qualifier(), nullptr);
        if (!_symtab.insert(id, symbol))
          throw std::string("variable '" + id + "' redeclared");
        _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
      }
    }
  } else {
    // [ public | require ] tipo identificador
        
    if (size) {
      if (node->is_typed(cdk::TYPE_INT)) {
        if (!node->initializer()->is_typed(cdk::TYPE_INT))
          throw std::string("wrong type for initializer (integer expected).");
      } 
      else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) 
          throw std::string("wrong type for initializer (integer or double expected).");
      } 
      else if (node->is_typed(cdk::TYPE_STRING)) {
        if (!node->initializer()->is_typed(cdk::TYPE_STRING))
          throw std::string("wrong type for initializer (string expected).");
      } 
      else if (node->is_typed(cdk::TYPE_POINTER)) {
        if (!node->initializer()->is_typed(cdk::TYPE_POINTER))
          throw std::string("wrong type for initializer (pointer expected).");
      } 
      else if (node->is_typed(cdk::TYPE_UNSPEC)) {
        node->type(node->initializer()->type());
      }
      else {
        throw std::string("unknown type for initializer.");
      }
    }

    auto id = *node->identifiers()->front();
    std::shared_ptr<og::symbol> symbol = std::make_shared<og::symbol>(node->type(), id, false, node->qualifier(), nullptr);
    if (!_symtab.insert(id, symbol))
      throw std::string("variable '" + id + "' redeclared");
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_block_node(og::block_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void og::type_checker::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in allocation expression");

  if (_lvalueType) {
    auto lvpointer = cdk::reference_type_cast(_lvalueType);
    node->type(cdk::make_reference_type(4, lvpointer->referenced()));
  }
  else
    throw std::string("allocation expression must have an associated left value");
}

void og::type_checker::do_address_of_node(og::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::make_reference_type(4, node->lvalue()->type()));
  } else {
    throw std::string("wrong type in unary logical expression");
  }
}

void og::type_checker::do_identity_node(og::identity_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT) && !node->argument()->is_typed(cdk::TYPE_DOUBLE)) 
    throw std::string("integer expression expected in binary expression");

  node->type(node->argument()->type());
}

void og::type_checker::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);

  // sizeof stores the size in bytes (integer)
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void og::type_checker::do_index_node(og::index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->entity()->accept(this, lvl + 2);
  if (!node->entity()->is_typed(cdk::TYPE_POINTER))
    throw std::string("pointer index: left-value must be a pointer");

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT))
    throw std::string("pointer index: index must be an integer");

  auto pointer = cdk::reference_type_cast(node->entity()->type());
  node->type(pointer->referenced());
}

void og::type_checker::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  node->entity()->accept(this, lvl + 2);
  if (!node->entity()->is_typed(cdk::TYPE_STRUCT))
    throw std::string("tuple index: left-value must be a tuple");

  auto structure = cdk::structured_type_cast(node->entity()->type());
  node->type(structure->component(node->index() - 1));
}

//---------------------------------------------------------------------------

void og::type_checker::do_tuple_node(og::tuple_node *const node, int lvl) {
  node->expressions()->accept(this, lvl + 2);
  
  std::vector<std::shared_ptr<cdk::basic_type>> components;
  for (size_t i = 0; i < node->expressions()->size(); i++) {
     cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->expressions()->node(i));
     components.push_back(arg->type());
  }

  if (node->expressions()->size() > 1)
    node->type(cdk::make_structured_type(components));
  else {
    cdk::expression_node *expr = dynamic_cast<cdk::expression_node*>(node->expressions()->node(0));
    node->type(expr->type());
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_reference_type(4, node->type()));
}
