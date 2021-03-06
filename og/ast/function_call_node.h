#ifndef __OG_AST_FUNCTION_CALL_H__
#define __OG_AST_FUNCTION_CALL_H__

#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing function call nodes.
   *
   * If _arguments is null, them the node is either a call to a function with
   * no arguments (or in which none of the default arguments is present) or
   * an access to a variable.
   */
  class function_call_node: public cdk::expression_node {
    std::string _identifier;
    cdk::sequence_node *_arguments;

  public:
    /**
     * Constructor for a function call with arguments.
     */
    inline function_call_node(int lineno, const std::string &identifier, cdk::sequence_node *arguments) :
        cdk::expression_node(lineno), _identifier(identifier), _arguments(arguments) {
    }

  public:
    inline const std::string &identifier() const {
      return _identifier;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }
  };

} // og

#endif
