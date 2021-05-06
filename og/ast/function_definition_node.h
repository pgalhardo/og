#ifndef __OG_AST_FUNCTION_DEFINITION_NODE_H__
#define __OG_AST_FUNCTION_DEFINITION_NODE_H__

#include <string>
#include <cdk/ast/basic_node.h>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>

namespace og {

  /**
   * Class for describing function definitions.
   */
  class function_definition_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;
    cdk::basic_node *_block;

  public:
    inline function_definition_node(int lineno, int qualifier, cdk::basic_type *var_type,
                                    const std::string &identifier, cdk::sequence_node *arguments, cdk::basic_node *block) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _block(block) {
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
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }
  };

} // og

#endif
