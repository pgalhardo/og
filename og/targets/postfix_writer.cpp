#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include "ast/all.h"  // all.h is automatically generated
// must come after other #includes
#include "og_parser.tab.h"
//---------------------------------------------------------------------------

void og::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void og::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void og::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.DOUBLE(node->value());  // load number to the stack
  } else {
    _pf.SDOUBLE(node->value()); // double is on the DATA segment
  }
}
void og::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.NOT();
}
void og::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void og::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.INT(node->value());  // push an integer
  } else {
    _pf.SINT(node->value()); // integer literal is on the DATA segment
  }
}

void og::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  if (_inFunctionBody) {
    // local variable initializer
    _pf.TEXT(); // return to the TEXT segment
    _pf.ALIGN();
    _pf.ADDR(mklbl(lbl1)); // the string to be printed
  } else {
    // global variable initializer
    _pf.DATA();
    _pf.ALIGN();
    _pf.SADDR(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
  _pf.DADD();
  else
  _pf.ADD();
}
void og::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
  _pf.DSUB();
  else
  _pf.SUB();
}
void og::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DMUL();
  else
    _pf.MUL();
}
void og::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DDIV();
  else
    _pf.DIV();
}
void og::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void og::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
}
void og::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}
void og::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}
void og::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
}
void og::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}
void og::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  const std::string &id = node->name();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);
  if (symbol->is_typed(cdk::TYPE_STRUCT)) {
    if (symbol->global()) {
       _pf.ADDR(symbol->name() + std::to_string(_tuple_offset));
    }
    else if (_tuple_offset) {
      int jmp = 0;
      auto structure = cdk::structured_type_cast(symbol->type());
      for (int i = structure->length() - 1; i >= _tuple_offset; i--)
        jmp += structure->component(i)->size();
      _pf.LOCAL(symbol->offset() + jmp);
    } else {
      auto structure = cdk::structured_type_cast(symbol->type());
      int jmp = structure->size();
      for (size_t i = 0 ; i < structure->length(); i++) {
        jmp -= structure->component(i)->size();
        _pf.LOCAL(symbol->offset() + jmp);
        if (structure->component(i)->size() == 4)
          _pf.LDINT();
        else
          _pf.LDDOUBLE();
      }
    }
  } else {
    if (symbol->global()) {
       _pf.ADDR(symbol->name());
    }
    else {
      _pf.LOCAL(symbol->offset());
    }
  }
}

void og::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDDOUBLE();
  }
  else if (!node->lvalue()->is_typed(cdk::TYPE_STRUCT)) {
    // integers, pointers, and strings
    _pf.LDINT();
  }
}

void og::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->rvalue()->accept(this, lvl); // determine the new value
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT))
    _pf.I2D();
    _pf.DUP64();
    _dupped += 8;
  } else {
    _pf.DUP32();
    _dupped += 4;
  }

  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.STDOUBLE();
  } else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_evaluation_node(og::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.TRASH(8); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_POINTER)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_VOID)) {
    // EMPTY
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void og::postfix_writer::do_write_node(og::write_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print

  for (size_t i = 0; i < node->argument()->size(); i++) {
    cdk::expression_node *inode = dynamic_cast<cdk::expression_node*>(node->argument()->node(i));

    if (inode->is_typed(cdk::TYPE_INT)) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // delete the printed value
    } else if (inode->is_typed(cdk::TYPE_DOUBLE)) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // delete the printed value
    } else if (inode->is_typed(cdk::TYPE_STRING)) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // delete the printed value's address
    } else {
      std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
      exit(1);
    }
  }
  
  _functions_to_declare.insert("println");
  if (node->newline()) _pf.CALL("println"); // print a newline
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_input_node(og::input_node * const node, int lvl) {
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else if (node->is_typed(cdk::TYPE_INT)) {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  } else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
    return;
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_for_node(og::for_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _symtab.push(); // for local vars
  _forIni.push(++_lbl);  // after init, before body
  _forStep.push(++_lbl); // after init, before body
  _forEnd.push(++_lbl);  // after for

  if (node->inits()) {
    _dupped = 0;
    node->inits()->accept(this, lvl);
    if (_dupped)
      _pf.TRASH(_dupped);
  }

  _pf.LABEL(mklbl(_forIni.top()));
  if (node->condition())
    node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(_forEnd.top()));
  node->block()->accept(this, lvl + 2);

  _pf.LABEL(mklbl(_forStep.top()));
  if (node->incrs()) {
    _dupped = 0;
    node->incrs()->accept(this, lvl + 2);
    if (_dupped)
      _pf.TRASH(_dupped);
  }

  _pf.JMP(mklbl(_forIni.top()));
  _pf.LABEL(mklbl(_forEnd.top()));

  _forIni.pop();
  _forStep.pop();
  _forEnd.pop();
  _symtab.pop();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_if_node(og::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1; // TODO needed??
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_if_else_node(og::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_function_call_node(og::function_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  size_t argsSize = 0;
  std::shared_ptr<og::symbol> function = _symtab.find(node->identifier());
  cdk::sequence_node * func_arguments = function->arguments();
  
  if (node->arguments()) {
    for (int ax = node->arguments()->size(); ax > 0; ax--) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax - 1));
      cdk::typed_node *func_arg = dynamic_cast<cdk::typed_node*>(func_arguments->node(ax - 1));
      arg->accept(this, lvl + 2);

      if (func_arg->is_typed(cdk::TYPE_DOUBLE)
      && arg->is_typed(cdk::TYPE_INT))
        _pf.I2D();
        
      argsSize += func_arg->type()->size();
    }
  }

  if (function->is_typed(cdk::TYPE_STRUCT)) {
    // Return type is structured, we need to reserve a memory zone compatible with the return type
    // and pass it by pointer to the function called as its first argument

    if (_variable != nullptr) {
      _pf.LOCAL(_variable->offset());
      argsSize += 4;
    }
  }
  
  _pf.CALL(node->identifier());
  if (argsSize != 0) { _pf.TRASH(argsSize); }

  if (function->is_typed(cdk::TYPE_INT)
  || function->is_typed(cdk::TYPE_POINTER)
  || function->is_typed(cdk::TYPE_STRING)) {
    _pf.LDFVAL32();
  }
  else if (function->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDFVAL64();
  }
  else {
    // Return type is structured, data has been copied.
    _shouldCopy = false;
  }
}

void og::postfix_writer::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inFunctionArgs) {
    std::cerr << "ERROR: cannot declare function in body or in args!" << std::endl;
    return;
  }

  if (!new_symbol()) return;

  std::shared_ptr<og::symbol> function = new_symbol();
  _functions_to_declare.insert(function->name());
  reset_new_symbol();
}

void og::postfix_writer::do_function_definition_node(og::function_definition_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inFunctionArgs)
    throw "cannot define function in body or in arguments";

  // remember symbol so that args and body know
  _function = new_symbol();
  _functions_to_declare.erase(_function->name());
  reset_new_symbol();

  if (node->is_typed(cdk::TYPE_UNSPEC))
    _offset = 12;
  else
    _offset = 8;
  _symtab.push(); // scope of args
  if (node->arguments()) {
    _inFunctionArgs = true;
    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
      cdk::typed_node *arg = dynamic_cast<cdk::typed_node*>(node->arguments()->node(ix));
      if (arg == nullptr) break; // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
    }
    _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  
  if (node->qualifier() == tPUBLIC && node->identifier().compare("og") == 0) {
    // generate the main function (RTS mandates that its name be "_main")
    _pf.GLOBAL("_main", _pf.FUNC());
    _pf.LABEL("_main");
  }
  else if (node->identifier().compare("og") == 0) {
    // TODO throw exception: og must be declared public
  }
  else {
    if (node->qualifier() == tPUBLIC)
      _pf.GLOBAL(node->identifier(), _pf.FUNC());
    _pf.LABEL(node->identifier());
  }
  
  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables

  _offset = 0;
  _inFunctionBody = true;
  node->block()->accept(this, lvl + 4); // block has its own scope
  _inFunctionBody = false;
  _symtab.pop(); // scope of arguments

  // end the main function
  //TODO JMP here, single exit point
  _pf.LEAVE();
  _pf.RET();

  if (node->identifier() == "og") {
    // declare external functions
    for (std::string s : _functions_to_declare)
      _pf.EXTERN(s);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_return_node(og::return_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  if (!_function->is_typed(cdk::TYPE_VOID) && !_function->is_typed(cdk::TYPE_STRUCT)) {
    node->argument()->accept(this, lvl); // determine the value

    if (_function->is_typed(cdk::TYPE_INT)
    || _function->is_typed(cdk::TYPE_STRING)
    || _function->is_typed(cdk::TYPE_POINTER)) {
      _pf.STFVAL32();
    } else if (_function->is_typed(cdk::TYPE_DOUBLE)) {
      if (node->argument()->is_typed(cdk::TYPE_INT))
        _pf.I2D();
      _pf.STFVAL64();
    }
  }
  else if (_function->is_typed(cdk::TYPE_STRUCT)) {
    // Return type is structured, we need to copy the data to the reserved memory zone
    // SP @ 8
    
    auto structure = cdk::structured_type_cast(_function->type());
    og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->argument());
    
    for (size_t i = 0; i < tuple->expressions()->size(); i++) {
      cdk::expression_node *expr = dynamic_cast<cdk::expression_node*>(tuple->expressions()->node(i));
      expr->accept(this, lvl);

      if (expr->is_typed(cdk::TYPE_INT)
      && structure->component(i)->name() == cdk::TYPE_DOUBLE) {
        _pf.I2D();
      }
    }
    //node->argument()->accept(this, lvl); // determine the value

    int jmp = 0;

    for (int i = structure->length() - 1; i >= 0; i--) {
      _pf.LOCAL(8);
      _pf.LDINT();
      _pf.INT(jmp);
      _pf.ADD();
      if (structure->component(i)->size() == 4) {
        _pf.STINT();
      } else if (structure->component(i)->size() == 8) {
        _pf.STDOUBLE();
      }
      jmp += structure->component(i)->size();
    }
  }
  _pf.LEAVE();
  _pf.RET();
}

void og::postfix_writer::do_break_node(og::break_node * const node, int lvl) {
  if (_forEnd.size() != 0) {
    _pf.JMP(mklbl(_forEnd.top())); // jump to for end
  } else {
    std::cerr << "ERROR:'break' outside 'for'!" << std::endl;
    exit(-1);
  } 
}

void og::postfix_writer::do_continue_node(og::continue_node * const node, int lvl) {
  if (_forStep.size() != 0) {
    _pf.JMP(mklbl(_forStep.top())); // jump to for test
  } else {
    std::cerr << "ERROR:'continue' outside 'for'!" << std::endl;
    exit(-1);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // remember symbol so that function call knows its location
  _variable = new_symbol();

  size_t size;
  if (node->initializer() != nullptr) {
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

  int offset = 0, typesize = node->type()->size();
  if (_inFunctionBody) {
    // local variable
    _offset -= typesize;
    offset = _offset;
  } else if (_inFunctionArgs) {
    // function argument
    offset = _offset;
    _offset += typesize;
  } else {
    // global variable
    offset = 0;
  }

  std::shared_ptr<og::symbol> symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionBody) {
    // if we are dealing with local variables, then no action is needed
    // unless an initializer exists

    if (size > 1) {
      // [ public ] auto identificadores = expressões

      node->initializer()->accept(this, lvl);

      _pf.TEXT();
      _pf.ALIGN();

      if (node->identifiers()->size() == 1) {
        // auto x = 1, 2, 3;

        // Data has already been copied by a return_node.
        if (!_shouldCopy) { _variable = nullptr; _shouldCopy = true; return; }
        
        auto id = *node->identifiers()->back();
        symbol = _symtab.find(id);

        auto structure = cdk::structured_type_cast(symbol->type());
        
        int jmp = 0, base = symbol->offset();
        for (int i = structure->length() - 1; i >= 0; i--) {

          _pf.LOCAL(base);
          _pf.INT(jmp);
          _pf.ADD();
          if (structure->component(i)->size() == 4) {
            _pf.STINT();
          } else if (structure->component(i)->size() == 8) {
            _pf.STDOUBLE();
          }
          jmp += structure->component(i)->size();
          _offset -= structure->component(i)->size();
        }
      }
      else {
        // auto i, j, k = 3, 61.0, "string1";

        auto structure = cdk::structured_type_cast(node->initializer()->type());
        
        for (int i = structure->length() - 1; i >= 0; i--) {
          auto id = *node->identifiers()->at(i);
          
          symbol = _symtab.find(id);
          symbol->set_offset(_offset);

          if (_shouldCopy) {
            if (symbol) {
              _pf.LOCAL(symbol->offset());
              if (structure->component(i)->size() == 4) {
                _pf.STINT();
              } else if (structure->component(i)->size() == 8) {
                _pf.STDOUBLE();
              }
            } else {
              throw std::string(id + " is undeclared!");
            }
          }
          _offset += structure->component(i)->size();
        }
      }
    } 
    else if (size == 1) {
      // [ public | require ] tipo identificador = expressão

      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
          if (node->initializer()->is_typed(cdk::TYPE_INT))
            _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      } else if (node->is_typed(cdk::TYPE_STRUCT)) {
        // Data has been copied.
      } else {
        std::cerr << "cannot initialize" << std::endl;
        exit(-1);
      }
    }
  }
  else if (!_inFunctionArgs) {
    // Global variable

    if (size > 1) {
      // [ public ] auto identificadores = expressões
      
      if (node->identifiers()->size() == 1) {
        // auto x = 1, 2, 3;

        int lbl = 0;
        auto id = *node->identifiers()->at(0);
        og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->initializer());
        cdk::sequence_node *inits = tuple->expressions();

        _pf.DATA();
        _pf.ALIGN();

        for (size_t i = 0; i < size; i++) {
          cdk::expression_node *init = dynamic_cast<cdk::expression_node*>(inits->node(i));
          std::string label = id + std::to_string(++lbl);
          
          if (node->qualifier() == tPUBLIC)
            _pf.GLOBAL(label, _pf.OBJ());
          _pf.LABEL(label);
          init->accept(this, lvl);
        }
      }
      else {
        // auto i, j, k = 3, 61.0, "string1";
        
        og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->initializer());
        cdk::sequence_node *inits = tuple->expressions();

        _pf.DATA();
        _pf.ALIGN();
        for (size_t i = 0; i < size; i++) {
          auto id = *node->identifiers()->at(i);
          cdk::expression_node *init = dynamic_cast<cdk::expression_node*>(inits->node(i));

          if (node->qualifier() == tPUBLIC)
            _pf.GLOBAL(id, _pf.OBJ());
          _pf.LABEL(id);
          init->accept(this, lvl);

          offset += init->type()->size();
          symbol = new_symbol();
          if (symbol) {
            symbol->set_offset(offset);
            reset_new_symbol();
          }
        }
      }
    }
    else {
      // [ public | require ] tipo identificador [ = expressão ]
      auto id = *node->identifiers()->at(0);

      if (node->initializer() == nullptr) {
        // [ public | require ] tipo identificador
        _pf.BSS();
        _pf.ALIGN();
        if (node->qualifier() == tPUBLIC)
            _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
        
        if (node->is_typed(cdk::TYPE_INT)
        || node->is_typed(cdk::TYPE_POINTER)
        || node->is_typed(cdk::TYPE_STRING))
          _pf.SINT(0);
        else if (node->is_typed(cdk::TYPE_DOUBLE))
          _pf.SDOUBLE(0);

      } else {
        // [ public | require ] tipo identificador = expressão

        _pf.DATA();
        _pf.ALIGN();
        if (node->qualifier() == tPUBLIC)
          _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);

        if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_POINTER)) {
          node->initializer()->accept(this, lvl);
        } 
        else if (node->is_typed(cdk::TYPE_DOUBLE)) {
          if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
            node->initializer()->accept(this, lvl);
          } else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
            cdk::integer_node *dclini = dynamic_cast<cdk::integer_node *>(node->initializer());
            cdk::double_node ddi(dclini->lineno(), dclini->value());
            ddi.accept(this, lvl);
          } else {
            std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
            exit(-1);
          }
        }
        else if (node->is_typed(cdk::TYPE_STRING)) {
          node->initializer()->accept(this, lvl);
        }
      }
    }
  }
  _variable = nullptr; _shouldCopy = true;
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_block_node(og::block_node *const node, int lvl) {
  _symtab.push(); // for block-local vars
  if (node->declarations()) node->declarations()->accept(this, lvl + 4);
  if (node->instructions()) node->instructions()->accept(this, lvl + 4);
  _symtab.pop();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  auto pointer = cdk::reference_type_cast(node->type());
  _pf.INT(pointer->referenced()->size());
  _pf.MUL();
  _pf.ALLOC(); // allocate
  _pf.SP();    // put base pointer in stack
}

void og::postfix_writer::do_address_of_node(og::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl + 2);
}

void og::postfix_writer::do_identity_node(og::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
}

void og::postfix_writer::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
        _pf.INT(node->argument()->type()->size());
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_index_node(og::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->entity()->accept(this, lvl);
  node->index()->accept(this, lvl);
  auto pointer = cdk::reference_type_cast(node->entity()->type());
  _pf.INT(pointer->referenced()->size());
  _pf.MUL();
  _pf.ADD(); // add pointer and index
}

void og::postfix_writer::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _tuple_offset = node->index();
  node->entity()->accept(this, lvl);
  _tuple_offset = 0;
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_tuple_node(og::tuple_node *const node, int lvl) {
  node->expressions()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody) {
    _pf.INT(0);
  } else {
    _pf.SINT(0);
  }
}
