#ifndef __OG_TARGETS_POSTFIX_WRITER_H__
#define __OG_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <set>
#include <stack>          // std::stack
#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>

namespace og {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<og::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    int _tuple_offset;
    int _offset; // current framepointer offset (0 means no vars defined)
    int _dupped; // amount of bytes dupped
    std::stack<int> _forIni, _forStep, _forEnd; // for break/continue
    std::shared_ptr<og::symbol> _variable; // for keeping track of the current variable
    std::shared_ptr<og::symbol> _function; // for keeping track of the current function and its arguments
    std::set<std::string> _functions_to_declare;

    // semantic analysis
    bool _inFunction, _inFunctionBody, _inFunctionArgs, _shouldCopy;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<og::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _tuple_offset(0), _offset(0), _dupped(0),
        _variable(nullptr), _function(nullptr), _inFunction(false), _inFunctionBody(false), _inFunctionArgs(false), _shouldCopy(true) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // og

#endif
