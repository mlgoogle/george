// Copyright (c) 2016 The george Authors. All rights reserved.
// stock.h
// Created on: 2016年6月6日.
// Author: Paco.
#ifndef PLUGINS_SEARCH_SEARCH_H_
#define PLUGINS_SEARCH_SEARCH_H_

#include <string>
#include <algorithm>
#include "search/mutexlock.h"
#include "pub/net/typedef.h"

namespace search {

class Stock {
 public:
  Stock();
  ~Stock();
  Stock(const Stock& stock);
  Stock& operator = (const Stock& stock);

  void Serialization(DicValue* value);

  std::string code() { return data_->code_; }
  std::string name() { return data_->name_; }
  std::string sef_spell() { return data_->sef_spell_; }
  std::string stock_type() { return data_->stock_type_; }
  std::string sim_spell() { return data_->sim_spell_; }

  void set_code(std::string code) { data_->code_ = code; }
  void set_name(std::string name) { data_->name_ = name; }
  void set_sef_spell(std::string spell) {
    std::transform(spell.begin(), spell.end(), spell.begin(), toupper);
    data_->sef_spell_ = spell; }
  void set_sim_spell(std::string spell) {
      std::transform(spell.begin(), spell.end(), spell.begin(), toupper);
      data_->sim_spell_ = spell; }
  void set_stock_type(std::string type) { data_->stock_type_ = type; }
class Data {
 public:
  Data() : refs_(1) {};

 public:
  std::string stock_type_;
  std::string code_;
  std::string name_;
  std::string sef_spell_;
  std::string sim_spell_;
  void AddRef() {
    MutexLock lock(&refs_mutex_);
    ++refs_;
  }

  void Release() {
    bool do_delete = false;
    MutexLock lock(&refs_mutex_);
    --refs_;
    assert(refs_ >= 0);
    if (refs_ <= 0) {
      do_delete = true;
    }
    if (do_delete) {
      delete this;
    }
  }

 private:
  int refs_;
  Mutex refs_mutex_;
};

 private:
  Data* data_;
};

class StockPrice {
 public:
  StockPrice();
  ~StockPrice();
  StockPrice(const StockPrice& stock);
  StockPrice& operator = (const StockPrice& stock);

  void Serialization(DicValue* value);

  std::string code() { return data_->code_; }
  double cur_price() { return data_->cur_price_; }
  double close_price() { return data_->close_price_; }
  double status_type() { return data_->status_type_; }
  double change_percent() { return data_->change_percent_; }

  void set_code(std::string code) { data_->code_ = code; }
  void set_cur_price(double price) { data_->cur_price_ = price; }
  void set_close_price(double price) { data_->close_price_ = price; }
  void set_status_type(int64 type) { data_->status_type_ = type; }
  void set_change_percent(double per) { data_->change_percent_ = per; }
class Data {
 public:
  Data() : refs_(1), cur_price_(0.0), change_percent_(0.0),
      close_price_(0.0), status_type_(-1) {};

 public:
  std::string code_;
  double cur_price_;
  double change_percent_;
  double close_price_;
  int64 status_type_;
  void AddRef() {
    MutexLock lock(&refs_mutex_);
    ++refs_;
  }

  void Release() {
    bool do_delete = false;
    MutexLock lock(&refs_mutex_);
    --refs_;
    assert(refs_ >= 0);
    if (refs_ <= 0) {
      do_delete = true;
    }
    if (do_delete) {
      delete this;
    }
  }

 private:
  int refs_;
  Mutex refs_mutex_;
};

 private:
  Data* data_;
};

}  // namespace search



#endif  // PLUGINS_SEARCH_SEARCH_H_
