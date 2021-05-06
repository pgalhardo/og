#ifndef __OG_AST_TUPLE_INDEX_NODE_H__
#define __OG_AST_TUPLE_INDEX_NODE_H__

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing tuple indexing nodes.
   */
  class tuple_index_node: public cdk::lvalue_node {
    cdk::expression_node *_entity;
    int _index;

  public:
    inline tuple_index_node(int lineno, cdk::expression_node *entity, int index) :
        lvalue_node(lineno), _entity(entity), _index(index) {
    }

  public:
    inline cdk::expression_node *entity() {
        return _entity;
    }
    inline int index() const {
        return _index;
    }

    void accept(basic_ast_visitor *sp, int level) override {
        return sp->do_tuple_index_node(this, level);
    }
  };

} // og

#endif
