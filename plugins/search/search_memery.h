// Copyright (c) 2016 The george Authors. All rights reserved.
// news_memery.h
// Created on: 2016年5月18日.
// Author: Paco.
#ifndef PLUGINS_SEARCH_SEARCH_MEMERY_H_
#define PLUGINS_SEARCH_SEARCH_MEMERY_H_

#include <stdio.h>
#include <string.h>

#include "search/search_mysql.h"
#include "search/search_proto_buf.h"
#include "search/search_base.h"
#include "base/thread/base_thread_lock.h"


namespace search {

class SearchMemery {
 public:
  static SearchMemery* GetInstance();

  int UpdateCacheNews();
  void InitSearchMysql(SearchMysql* ptr);
  void InitAllStock();

  void QuerySearchStock(int64 uid, std::string key_name,
                        send::SearchList* list);

  void AlterSubscribe(int64 uid, int64 type, std::string code);

  void QueryHotSubscribe(int64 uid, send::SearchList* list);
  void QueryUserSubscribe(int64 uid, send::SearchList* list);

  void UpdateStockPrice();

  SearchMemery();
  ~SearchMemery();

 private:
  void SetSendStock(send::SendStock* send_stock, Stock stock,
                    std::string sub_str);

  StockMap stock_map_;
  StockPriceMap price_map_;

  //由search_interface传入，该类不负责管理，只使用
  SearchMysql* search_mysql_;

  threadrw_t*  lock_;


 private:
  static SearchMemery* instance_;

};

}  // namespace search
#endif  // PLUGINS_SEARCH_SEARCH_MEMERY_H_
