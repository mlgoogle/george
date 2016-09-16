// Copyright (c) 2016 The george Authors. All rights reserved.
// news_memery.cc
// Created on: 2016年5月18日.
// Author: Paco.


#include "search/search_memery.h"

#include <assert.h>
#include <regex.h>
#include <algorithm>

#include "tools/tools.h"

#include "logic/logic_comm.h"

//base_logic::RLockGd lk(lock_);
//base_logic::WLockGd lk(lock_);
#define SEARCH_LIST_SIZE 10
namespace search {

SearchMemery* SearchMemery::instance_;

SearchMemery::SearchMemery() {
  search_mysql_ = NULL;
  InitThreadrw(&lock_);
}

SearchMemery::~SearchMemery() {
  DeinitThreadrw(lock_);
}

SearchMemery* SearchMemery::GetInstance() {
  if (NULL == instance_)
    instance_ = new SearchMemery();
  return instance_;
}

void SearchMemery::InitSearchMysql(SearchMysql* ptr) {
  search_mysql_ = ptr;
}

void SearchMemery::InitAllStock() {
  search_mysql_->QueryAllStock(&stock_map_);
  LOG_DEBUG2("stock_map_size[%d]", stock_map_.size());
}

void SearchMemery::UpdateStockPrice() {
  search_mysql_->QueryStockPrice(&price_map_);
}

void SearchMemery::QueryUserSubscribe(int64 uid, send::SearchList* list) {
  std::string sub_str;
  search_mysql_->QuerySubScribe(uid, &sub_str);
  std::vector<std::string> code_vec = tools::Split(sub_str, ",");
  std::vector<std::string>::iterator it = code_vec.begin();
  for (; it != code_vec.end(); ++it) {
    LOG_DEBUG("sub_str1111");
    send::SendStockPrice* send = new send::SendStockPrice();
    StockMap::iterator sm_it = stock_map_.find(*it);
    if (sm_it != stock_map_.end()) {
      send->set_code(*it);
      Stock stock = sm_it->second;
      send->set_name(stock.name());
      send->set_stock_type(stock.stock_type());
    }
    StockPriceMap::iterator sp_it = price_map_.find(*it);
    if (sp_it != price_map_.end()) {
      StockPrice price = sp_it->second;
      send->set_cur_price(price.cur_price());
      send->set_close_price(price.close_price());
      send->set_change_percent(price.change_percent());
      send->set_status_type(price.status_type());
    }
    list->set_stock(send->get());
  }
}

void SearchMemery::QuerySearchStock(int64 uid, std::string key_name,
                                    send::SearchList* list) {
  std::string sub_str;
  search_mysql_->QuerySubScribe(uid, &sub_str);
  StockMap::iterator it = stock_map_.begin();
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), toupper);
  for (; it != stock_map_.end(); ++it) {
    Stock stock = it->second;
    if (stock.code().find(key_name) != std::string::npos) {
      send::SendStock* send_stock = new send::SendStock();
      SetSendStock(send_stock, stock, sub_str);
      list->set_stock(send_stock->get());
      if (list->size() >= SEARCH_LIST_SIZE)
        break;
      continue;
    } else if (stock.name().find(key_name) != std::string::npos) {
      send::SendStock* send_stock = new send::SendStock();
      SetSendStock(send_stock, stock, sub_str);
      list->set_stock(send_stock->get());
      if (list->size() >= SEARCH_LIST_SIZE)
        break;
      continue;
    } else if (stock.sim_spell().find(key_name) != std::string::npos) {
      send::SendStock* send_stock = new send::SendStock();
      SetSendStock(send_stock, stock, sub_str);
      list->set_stock(send_stock->get());
      if (list->size() >= SEARCH_LIST_SIZE)
        break;
      continue;
    }else if (stock.sef_spell().find(key_name) != std::string::npos) {
        send::SendStock* send_stock = new send::SendStock();
        SetSendStock(send_stock, stock, sub_str);
        list->set_stock(send_stock->get());
        if (list->size() >= SEARCH_LIST_SIZE)
          break;
        continue;
      }
  }
}

void SearchMemery::AlterSubscribe(int64 uid, int64 type, std::string code) {
  std::string sub_str;
  search_mysql_->QuerySubScribe(uid, &sub_str);
  //0-取消订阅 1-新增订阅
  if (type == 0) {
    std::string::size_type pos = sub_str.find(code);
    if (pos != std::string::npos) {
      if (pos + 6 == sub_str.length() && pos == 0) {
          sub_str = "";
      } else if (pos != 0){
          sub_str.replace(pos-1, 7, "");
      } else {
          sub_str.replace(pos, 7, "");
      }
      search_mysql_->DelSubscribe(uid, code, sub_str);
    }
  } else if (type == 1) {
    std::string::size_type pos = sub_str.find(code);
    if (pos == std::string::npos) {
      if (sub_str.length() != 0)
        sub_str.append(",");
      sub_str.append(code);
      search_mysql_->AddSubscribe(uid, code, sub_str);
    }
  }
}

void SearchMemery::QueryHotSubscribe(int64 uid, send::SearchList* list) {
  std::vector<std::string> out;
  search_mysql_->QueryHotSubscribe(&out);
  std::vector<std::string>::iterator it = out.begin();
  for (; it != out.end(); it++) {
      StockMap::iterator m_it = stock_map_.find(*it);
      if (m_it != stock_map_.end()) {
        send::SendStock* send_stock = new send::SendStock();
        SetSendStock(send_stock, m_it->second, "");
        list->set_stock(send_stock->get());
      }
  }
}

void SearchMemery::SetSendStock(send::SendStock* send_stock, Stock stock,
                         std::string sub_str) {
  LOG_DEBUG2("sub_str[%s]--stock_code[%s]", sub_str.c_str(), stock.code().c_str());
  send_stock->set_code(stock.code());
  send_stock->set_name(stock.name());
  send_stock->set_stock_type(stock.stock_type());
  if (sub_str.find(stock.code()) != std::string::npos) {
      LOG_DEBUG2("find on [%d]", sub_str.find(stock.code()));
    send_stock->set_subscribe(1);
  } else {
    LOG_DEBUG2("not find [%d]", sub_str.find(stock.code()));
    send_stock->set_subscribe(0);
  }
}

}  // namespace search
