#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

// must come after other #includes
#include "og_parser.tab.h"

//---------------------------------------------------------------------------

void og::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void og::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void og::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}
void og::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}
void og::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void og::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void og::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void og::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl);
  reset_new_symbol();

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}


//---------------------------------------------------------------------------

void og::xml_writer::do_evaluation_node(og::evaluation_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_write_node(og::write_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_input_node(og::input_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_for_node(og::for_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("initial", lvl + 2);
  node->inits()->accept(this, lvl + 4);
  closeTag("initial", lvl + 2);

  openTag("conditions", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("conditions", lvl + 2);

  openTag("incrs", lvl + 2);
  node->incrs()->accept(this, lvl + 4);
  closeTag("incrs", lvl + 2);
  
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_if_node(og::if_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);

  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_if_else_node(og::if_else_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);

  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);

  if (node->elseblock()) {
    openTag("else", lvl);
    node->elseblock()->accept(this, lvl + 4);
    closeTag("else", lvl);
  }
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------
static std::string get_qualifier(int qualifier) {
  if (qualifier == tPUBLIC) return "public";
  if (qualifier == tREQUIRE) return "require";
  return "private";
}

void og::xml_writer::do_function_call_node(og::function_call_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "'>" << std::endl;
  openTag("arguments", lvl);
  if (node->arguments())
    node->arguments()->accept(this, lvl + 4);
  closeTag("arguments", lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " qualifier='"<< get_qualifier(node->qualifier()) \
                        << "' name='" << node->identifier() << "' type='"<< cdk::to_string(node->type()) \
                        << "'>" << std::endl;
   openTag("arguments", lvl);

  if (node->arguments())
    node->arguments()->accept(this, lvl + 4);
  closeTag("arguments", lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_function_definition_node(og::function_definition_node *const node, int lvl) {
  os() << std::string(lvl, ' ')  << "<" << node->label() << " qualifier='"<< get_qualifier(node->qualifier()) \
                        << "' name='" << node->identifier() << "' type='"<< cdk::to_string(node->type()) \
                        << "'>" << std::endl;

  openTag("arguments", lvl);
  if (node->arguments())
    node->arguments()->accept(this, lvl + 4);
  closeTag("arguments", lvl);

  openTag("block", lvl);
  if (node->block())
    node->block()->accept(this, lvl + 4);
  closeTag("block", lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_return_node(og::return_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  if (node->argument()) {
    openTag("return_expression", lvl + 2);
    node->argument()->accept(this, lvl + 4);
    closeTag("return_expression", lvl + 2);
  }
  closeTag(node, lvl);
}

void og::xml_writer::do_break_node(og::break_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_continue_node(og::continue_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {
   //TODO CHECK_TYPES or ASSERT_SAFE
    os() << std::string(lvl, ' ')  << "<" << node->label() << " qualifier='" << get_qualifier(node->qualifier()) \
                                << "' type='"<< cdk::to_string(node->type()) << "'>" << std::endl;

    openTag("identifiers", lvl);
    std::vector<std::string *> *identifiers = node->identifiers();
    for (std::string *i: *identifiers) {
      os() << std::string(lvl + 4, ' ') << "<identifier name='" << *i  << "'/>" << std::endl; 
    }
    closeTag("identifiers", lvl);

    if (node->initializer() != NULL) {
      openTag("initializer", lvl+2);
      node->initializer()->accept(this, lvl+4);
      closeTag("initializer", lvl+2);
    }
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_block_node(og::block_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("declarations", lvl + 2);
  if (node->declarations())
    node->declarations()->accept(this, lvl + 4);
  closeTag("declarations", lvl + 2);

  openTag("instructions", lvl + 2);
  if (node->instructions())
    node->instructions()->accept(this, lvl + 4);
  closeTag("instructions", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}

void og::xml_writer::do_address_of_node(og::address_of_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("lvalue", lvl + 2);
  node->lvalue()->accept(this, lvl + 4);
  closeTag("lvalue", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_identity_node(og::identity_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}

void og::xml_writer::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_index_node(og::index_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("entity", lvl + 2);
  node->entity()->accept(this, lvl + 4);
  closeTag("entity", lvl + 2);

  openTag("index", lvl + 2);
  node->index()->accept(this, lvl + 4);
  closeTag("index", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("entity", lvl + 2);
  node->entity()->accept(this, lvl + 4);
  closeTag("entity", lvl + 2);

  openTag("index", lvl + 2);
  os() << std::string(lvl + 4 , ' ') << "<position at=" << node->index() << ">" << std::endl;
  closeTag("index", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_tuple_node(og::tuple_node *const node, int lvl) {
  openTag(node, lvl);
  node->expressions()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
