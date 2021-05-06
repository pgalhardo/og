#ifndef __OG_AST_VARIABLE_DECLARATION_NODE_H__
#define __OG_AST_VARIABLE_DECLARATION_NODE_H__

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>

namespace og {

  /**
  * Class for describing variable declaration nodes.
  */
  class variable_declaration_node : public cdk::typed_node {
    int _qualifier;
    std::vector<std::string *> *_identifiers;
    cdk::expression_node *_initializer;

  public:
    inline variable_declaration_node(int lineno, int qualifier, cdk::basic_type *var_type,
                                    std::vector<std::string *> *identifiers, cdk::expression_node *initializer) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifiers(identifiers), _initializer(initializer) {
      type(std::shared_ptr<cdk::basic_type>(var_type));
    }

  public:
    inline int qualifier() const {
      return _qualifier;
    }
    inline std::vector<std::string *> *identifiers() {
      return _identifiers;
    }
    inline cdk::expression_node *initializer() {
      return _initializer;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }
  };

} // og

#endif
