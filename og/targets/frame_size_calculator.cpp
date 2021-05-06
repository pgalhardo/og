#include <string>
#include "targets/frame_size_calculator.h"
#include "targets/symbol.h"
#include "ast/all.h"

og::frame_size_calculator::~frame_size_calculator() {
  os().flush();
}

void og::frame_size_calculator::do_add_node(cdk::add_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_and_node(cdk::and_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_div_node(cdk::div_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_double_node(cdk::double_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_eq_node(cdk::eq_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_ge_node(cdk::ge_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_gt_node(cdk::gt_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_integer_node(cdk::integer_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_le_node(cdk::le_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_lt_node(cdk::lt_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_mod_node(cdk::mod_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_mul_node(cdk::mul_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_neg_node(cdk::neg_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_ne_node(cdk::ne_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_not_node(cdk::not_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_or_node(cdk::or_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    cdk::basic_node *n = node->node(i);
    if (n == nullptr) break;
    n->accept(this, lvl + 2);
  }
}
void og::frame_size_calculator::do_string_node(cdk::string_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_sub_node(cdk::sub_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_variable_node(cdk::variable_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_address_of_node(og::address_of_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_block_node(og::block_node *const node, int lvl) {
  _symtab.push(); // for block-local vars
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}
void og::frame_size_calculator::do_break_node(og::break_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_continue_node(og::continue_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_evaluation_node(og::evaluation_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_for_node(og::for_node *const node, int lvl) {
  _symtab.push(); // for block-local vars
  node->inits()->accept(this, lvl + 2);
  _symtab.pop();
}
void og::frame_size_calculator::do_function_call_node(og::function_call_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_function_definition_node(og::function_definition_node *const node, int lvl) {
  _symtab.push(); // for block-local vars
  node->block()->accept(this, lvl + 2);
  _symtab.pop();

  if (_return_types.size() > 1) {

    if (!node->is_typed(cdk::TYPE_UNSPEC)) {
      for (size_t i = 1; i < _return_types.size(); i++)
        if (_return_types.at(i) != node->type())
          throw std::string("unexpected return type.");
    }
    else {
      bool diff = false;
      std::vector<std::shared_ptr<cdk::basic_type>> components;
      std::shared_ptr<cdk::basic_type> front = _return_types.front();
      auto front_structure = cdk::structured_type_cast(front);
      
      for (size_t i = 0; i < front_structure->length(); i++) {
        for (size_t j = 1; j < _return_types.size(); j++) {
          auto structure = cdk::structured_type_cast(_return_types.at(j));

          if (structure->component(i) != front_structure->component(i)) {
            if ((structure->component(i)->name() == cdk::TYPE_INT && front_structure->component(i)->name() == cdk::TYPE_DOUBLE)
            || (structure->component(i)->name() == cdk::TYPE_DOUBLE && front_structure->component(i)->name() == cdk::TYPE_INT))
              diff = true;
            else
              throw std::string("return types must match.");
          }
        }
        if (!diff) {
          components.push_back(front_structure->component(i));
        } else if (diff) {
          if (front_structure->component(i)->name() == cdk::TYPE_INT) {
            components.push_back(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
          }
          else if (front_structure->component(i)->name() == cdk::TYPE_DOUBLE) {
            components.push_back(front_structure->component(i));
          }
        }
        diff = false;
      }
      node->type(cdk::make_structured_type(components));
      std::shared_ptr<og::symbol> function = _symtab.find(node->identifier());
      if (function != nullptr) {
        function->type(node->type());
      } else {
        throw std::string("undeclared function '" + node->identifier() + "'");
      }
    }
  } 
  if (_return_types.size() && node->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(_return_types.front());
    
    std::shared_ptr<og::symbol> function = _symtab.find(node->identifier());
    if (function != nullptr) {
      function->type(node->type());
    } else {
      throw std::string("undeclared function '" + node->identifier() + "'");
    }
  }
  else if (_return_types.size() && node->is_typed(cdk::TYPE_VOID)) {
    throw std::string("return specified for void function.");
  }
}
void og::frame_size_calculator::do_identity_node(og::identity_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_if_else_node(og::if_else_node *const node, int lvl) {
  node->thenblock()->accept(this, lvl + 2);
  if (node->elseblock()) node->elseblock()->accept(this, lvl + 2);
}
void og::frame_size_calculator::do_if_node(og::if_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}
void og::frame_size_calculator::do_index_node(og::index_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_input_node(og::input_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_return_node(og::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _return_types.push_back(node->argument()->type());
}
void og::frame_size_calculator::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_tuple_node(og::tuple_node *const node, int lvl) {
  // EMPTY
}
void og::frame_size_calculator::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (!node->is_typed(cdk::TYPE_STRUCT)) {
    _localsize += node->type()->size();
  }
  else {
    auto structure = cdk::structured_type_cast(node->type());
    _localsize += structure->size();
  }
}
void og::frame_size_calculator::do_write_node(og::write_node *const node, int lvl) {
  // EMPTY
}
