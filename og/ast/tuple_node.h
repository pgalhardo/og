#ifndef __OG_AST_TUPLE_NODE_H__
#define __OG_AST_TUPLE_NODE_H__

#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing tuple nodes.
   */
 class tuple_node: public cdk::expression_node {
    cdk::sequence_node *_expressions;

  public:
    inline tuple_node(int lineno, cdk::sequence_node *expressions) :
        cdk::expression_node(lineno), _expressions(expressions) {
    }

  public:
    inline cdk::sequence_node *expressions() {
        return _expressions;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_tuple_node(this, level);
    }
  };

} // og

#endif
