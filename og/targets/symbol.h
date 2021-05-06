#ifndef __OG_TARGETS_SYMBOL_H__
#define __OG_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace cdk {
  class sequence_node;
}

namespace og {

  class symbol {
    int _qualifier;
    int _offset = 0; // 0 (zero) means global variable/function
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    bool _function;  // false for variables
    cdk::sequence_node *_arguments; // exclusive for functions, allows argument validation on function call

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, bool function, int qualifier, cdk::sequence_node *arguments) :
        _qualifier(qualifier), _type(type), _name(name), _function(function), _arguments(arguments) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    void type(std::shared_ptr<cdk::basic_type> type) {
      _type = type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
    int qualifier() const {
      return _qualifier;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }
    bool global() const {
      return _offset == 0;
    }
    bool isFunction() const {
      return _function;
    }
    cdk::sequence_node *arguments() {
      return _arguments;
    }
  };

} // og

#endif
