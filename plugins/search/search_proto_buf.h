// Copyright (c) 2016 The george Authors. All rights reserved.
// news_proto_buf.h
// Created on: 2016年5月30日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_PROTO_BUF_H_
#define PLUGINS_NEWS_CHECK_NEWS_PROTO_BUF_H_

#include "base/logic/base_values.h"

#include "pub/net/proto_buf.h"
#include "pub/net/typedef.h"

namespace search {

namespace recv {

class BaseRecv : public george_logic::PacketHead {
public:
  BaseRecv() {
   uid_ = -1;
 }
 virtual ~BaseRecv() {}

public:
 virtual void set_http_packet(DicValue* value);

 void set_uid(const int64 uid) { uid_ = uid; }

 void set_token(const std::string token) { token_ = token; }

 std::string token() { return token_; }
 int64 uid() { return uid_; }

private:
 int64 uid_;
 std::string token_;
};

class AlterSubscribe : public george_logic::PacketHead {
 public:
  AlterSubscribe() {
   uid_ = -1;
   alter_type_ = -1;
  }
  virtual ~AlterSubscribe() {}

public:
 virtual void set_http_packet(DicValue* dic);

 void set_uid(const int64 uid) { uid_ = uid; }

 void set_token(const std::string token) { token_ = token; }

 void set_stock(const std::string stock) { stock_ = stock; }

 void set_alter_type(const int64 type) { alter_type_ =  type; }

 std::string token() { return token_; }
 int64 uid() { return uid_; }
 std::string stock() { return stock_; }
 int64 alter_type() { return alter_type_; }

private:
 int64 uid_;
 int64 alter_type_;
 std::string token_;
 std::string stock_;
};

class SearchKey : public george_logic::PacketHead {
 public:
  SearchKey() {
    uid_ = -1;
  }
  virtual ~SearchKey() {}

 public:
  virtual void set_http_packet(DicValue* value);

  void set_uid(const int64 uid) { uid_ = uid; }

  void set_token(const std::string token) { token_ = token; }

  void set_key_name(const std::string name) { key_name_ = name; }

  std::string token() { return token_; }
  int64 uid() { return uid_; }
  std::string key_name() { return key_name_; }

 private:
  int64 uid_;
  std::string token_;
  std::string key_name_;
};

}  // namespace recv

namespace send {

class SendStockPrice {
 public:
  SendStockPrice() {
    code_ = NULL;
    name_ = NULL;
    stock_type_ = NULL;
    cur_price_ = NULL;
    change_percent_ = NULL;
    close_price_ = NULL;
    status_type_ = NULL;
    value_ = NULL;
  }
  ~SendStockPrice() {
    if (value_) { delete value_; value_ = NULL; }
  }

  void set_code(const std::string& code) {
    code_ = new base_logic::StringValue(code);
  }
  void set_name(const std::string& name) {
    name_ = new base_logic::StringValue(name);
  }
  void set_stock_type(const std::string& type) {
    stock_type_ = new base_logic::StringValue(type);
  }
  void set_cur_price(const double price) {
    cur_price_ = new base_logic::FundamentalValue(price);
  }

  void set_change_percent(const double percent) {
    change_percent_ = new base_logic::FundamentalValue(percent);
  }

  void set_close_price(const double price) {
    close_price_ = new base_logic::FundamentalValue(price);
  }

  void set_status_type(const int64 type) {
    status_type_ = new base_logic::FundamentalValue(type);
  }

  DicValue* get() {
    value_ = new DicValue();
    if (code_) value_->Set(L"code", code_);
    if (name_) value_->Set(L"name", name_);
    if (stock_type_) value_->Set(L"stock_type", stock_type_);
    if (cur_price_) value_->Set(L"cur_price", cur_price_);
    if (change_percent_) value_->Set(L"change_percent", change_percent_);
    if (close_price_) value_->Set(L"close_price", close_price_);
    if (status_type_) value_->Set(L"status_type", status_type_);

    return value_;
  }
 private:
  base_logic::StringValue* code_;
  base_logic::StringValue* name_;
  base_logic::StringValue* stock_type_;
  base_logic::FundamentalValue* cur_price_;
  base_logic::FundamentalValue* change_percent_;
  base_logic::FundamentalValue* close_price_;
  base_logic::FundamentalValue* status_type_;

  DicValue* value_;

};


class SendStock {
 public:
  SendStock() {
    code_ = NULL;
    name_ = NULL;
    stock_type_ = NULL;
    subscribe_ = NULL;
    value_ = NULL;
  }
  ~SendStock() {
    if (value_) { delete value_; value_ = NULL; }
  }

  void set_code(const std::string& code) {
    code_ = new base_logic::StringValue(code);
  }
  void set_name(const std::string& name) {
    name_ = new base_logic::StringValue(name);
  }
  void set_stock_type(const std::string& type) {
    stock_type_ = new base_logic::StringValue(type);
  }
  void set_subscribe(const int64 sub) {
    subscribe_ = new  base_logic::FundamentalValue(sub);
  }

  DicValue* get() {
    value_ = new DicValue();
    if (code_) value_->Set(L"code", code_);
    if (name_) value_->Set(L"name", name_);
    if (stock_type_) value_->Set(L"stock_type", stock_type_);
    if (subscribe_) value_->Set(L"subscribe", subscribe_);
    return value_;
  }
 private:
  base_logic::StringValue* code_;
  base_logic::StringValue* name_;
  base_logic::StringValue* stock_type_;
  base_logic::FundamentalValue* subscribe_;

  DicValue* value_;

};

class SearchList : public george_logic::PacketHead {
 public:
  SearchList() {
    list_ = new base_logic::ListValue();
    jsonpcallback_ = NULL;
  }

  virtual ~SearchList() {
    if (body_value_ != NULL) {
        delete body_value_;
        body_value_ = NULL;
    }
  }

  void set_stock(base_logic::Value* value) {
      list_->Append(value);
  }

  void set_jsonpcallback(const std::string& callback) {
    if (jsonpcallback_ == NULL) {
      jsonpcallback_ = new base_logic::StringValue(callback);
    }
  }

  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    body_value_->SetWithoutPathExpansion(L"list",list_);
    body_value_->SetInteger(L"count",list_->GetSize());
    if (jsonpcallback_ != NULL) {
      body_value_->Set(L"jsonpcallback",jsonpcallback_);
    }
    return body_value_;
  }

  int size() {
    return list_ == NULL ? 0 : list_->GetSize();
  }

  void set_http_packet(base_logic::DictionaryValue* value) {}

 private:
  base_logic::ListValue* list_;
  base_logic::StringValue* jsonpcallback_;
};

}  // namespace send

}  // namespace search



#endif  // PLUGINS_NEWS_CHECK_NEWS_PROTO_BUF_H_
