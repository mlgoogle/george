/*
 * observer.cpp
 *
 *  Created on: 2016年8月1日
 *      Author: Maibenben
 */

#include "Observer.h"

namespace stock_logic {

Observer::Observer() {
  // YGTODO Auto-generated constructor stub

}

Observer::~Observer() {
  // YGTODO Auto-generated destructor stub
}

std::string Observer::name() {
  return name_;
}
void Observer::set_name(std::string name) {
  name_ = name;
}

void Observer::Process(int socket, base_logic::DictionaryValue* params) {
  std::string result = Analysis(params);
  LOG_MSG2("result=%s", result.c_str());
  base_logic::LogicComm::SendFull(socket, result.c_str(),
                                  result.length());
}

} /* namespace stock_logic */
