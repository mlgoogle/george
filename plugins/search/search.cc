// Copyright (c) 2016 The george Authors. All rights reserved.
// stock.cc
// Created on: 2016年6月6日.
// Author: Paco.
#include "search.h"

namespace search {

Stock::Stock() {
  data_ = new Data();
}

Stock::~Stock() {

}

Stock::Stock(const Stock& stock) : data_(stock.data_){
  if (data_ != NULL) {
    data_->AddRef();
  }
}

Stock& Stock::operator = (const Stock& stock) {
  if(stock.data_ != NULL){
      stock.data_->AddRef();
  }
  if (data_ != NULL) {
    data_->Release();
  }
  data_ = stock.data_;
  return (*this);
}

void Stock::Serialization(DicValue* value) {
  value->GetString(L"code", &data_->code_);
  value->GetString(L"name", &data_->name_);
  value->GetString(L"sefspell", &data_->sef_spell_);
  value->GetString(L"stock_type", &data_->stock_type_);
}

StockPrice::StockPrice() {
  data_ = new Data();
}

StockPrice::~StockPrice() {

}

StockPrice::StockPrice(const StockPrice& stock) : data_(stock.data_){
  if (data_ != NULL) {
    data_->AddRef();
  }
}

StockPrice& StockPrice::operator = (const StockPrice& stock) {
  if(stock.data_ != NULL){
      stock.data_->AddRef();
  }
  if (data_ != NULL) {
    data_->Release();
  }
  data_ = stock.data_;
  return (*this);
}

void StockPrice::Serialization(DicValue* value) {
  value->GetString(L"code", &data_->code_);
  value->GetReal(L"cur_price", &data_->cur_price_);
  value->GetReal(L"close_price", &data_->close_price_);
  value->GetReal(L"change_percnet", &data_->change_percent_);
  value->GetBigInteger(L"status_type", &data_->status_type_);
}

}  // namespace search
