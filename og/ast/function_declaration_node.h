#ifndef __OG_AST_FUNCTION_DECLARATION_NODE_H__
#define __OG_AST_FUNCTION_DECLARATION_NODE_H__

#include <string>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>

namespace og {

  /**
   * Class for describing function declarations.
   */
  class function_declaration_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;

  public:
    inline function_declaration_node(int lineno, int qualifier, cdk::basic_type *var_type,
                                    const std::string &identifier, cdk::sequence_node *arguments) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments) {
      type(std::shared_ptr<cdk::basic_type>(var_type));
    }

  public:
    inline int qualifier() const {
      return _qualifier;
    }
    inline const std::string &identifier() const {
      return _identifier;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
  
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }
  };

} // og

#endif
