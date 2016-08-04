/*
 * observer.h
 *
 *  Created on: 2016年8月1日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_OBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_OBSERVER_H_

#include "Subject.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"

namespace stock_logic {

class Observer {
 public:
  Observer();
  virtual ~Observer();

  std::string name();
  void set_name(std::string name);

  virtual void Update(int opcode, stock_logic::Subject*)=0;

  void Process(int socket, base_logic::DictionaryValue* params);

  virtual std::string Analysis(base_logic::DictionaryValue* params) = 0;

  std::string name_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_OBSERVER_H_ */
