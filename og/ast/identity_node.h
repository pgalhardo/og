#ifndef __OG_AST_IDENTITY_NODE_H__
#define __OG_AST_IDENTITY_NODE_H__

#include <cdk/ast/unary_operation_node.h>

namespace og {

  /**
  * Class for describing identity operator nodes.
  */
  class identity_node: public cdk::unary_operation_node {
  
  public:
    inline identity_node(int lineno, cdk::expression_node *argument) :
        cdk::unary_operation_node(lineno, argument) {
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_identity_node(this, level);
    }
  };

} // og

#endif
