// Copyright (c) 2016 The george Authors. All rights reserved.
// search_mysql.h
// Created on: 2016年5月23日.
// Author: Paco.
#ifndef PLUGINS_SEARCH_SEARCH_MYSQL_H_
#define PLUGINS_SEARCH_SEARCH_MYSQL_H_

#include <vector>
#include <list>
#include <string>

#include "search/search_proto_buf.h"
#include "search/search_base.h"
#include "config/config.h"
#include "storage/data_engine.h"

namespace search {

class SearchMysql {
 public:
  SearchMysql(config::FileConfig* config);
  ~SearchMysql();

 public:
  int AddSubscribe(int64 uid, std::string code, std::string codes);
  int DelSubscribe(int64 uid, std::string code, std::string codes);
  int QueryAllStock(StockMap* map);
  int QuerySubScribe(int64 user_id, std::string* out);
  int QueryHotSubscribe(std::vector<std::string>* out);
  int QueryStockPrice(StockPriceMap* map);
  static void CallQueryAllStock(void* param, base_logic::Value* value);
  static void CallQuerySubScribe(void* param, base_logic::Value* value);
  static void CallQueryHotSubscribe(void* param, base_logic::Value* value);
  static void CallQueryStockPrice(void* param, base_logic::Value* value);
 private:
  base_logic::DataEngine* mysql_engine_;
};

}  // namespace search



#endif  // PLUGINS_SEARCH_SEARCH_MYSQL_H_
