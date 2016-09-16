// Copyright (c) 2016 The george Authors. All rights reserved.
// search_mysql.cc
// Created on: 2016年5月23日.
// Author: Paco.

#include "search/search_mysql.h"

#include <mysql/mysql.h>
#include <map>
#include <sstream>

#include "net/error_comm.h"
#include "logic/logic_comm.h"

namespace search {

SearchMysql::SearchMysql(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

SearchMysql::~SearchMysql() {
  if(mysql_engine_){
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

int SearchMysql::QueryAllStock(StockMap* map) {
  int err = 0;
  bool r = false;
  do {
    std::stringstream os;
    os << "call proc_QueryAllStocks();";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue = NULL;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQueryAllStock);
    if (!r) {
      LOG_ERROR("QueryAllStock sql error");
      err = SQL_EXEC_ERROR;
      break;
    }
    r = dict->GetList(L"resultvalue", &listvalue);
    if (!r || listvalue == NULL) {
      LOG_ERROR("QueryAllStock listvalue null");
      break;
    }
    while (listvalue->GetSize()) {
      search::Stock stock;
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      stock.Serialization(dict_result_value);
      map->insert(StockMap::value_type(stock.code(), stock));
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

int SearchMysql::QuerySubScribe(int64 user_id, std::string* out) {
  int err = 0;
  bool r = false;
  LOG_DEBUG("QuerySubScribe");
  do {
    std::stringstream os;
    os << "call proc_QuerySubScribe(" << user_id << ");";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue = NULL;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    LOG_DEBUG("11111");
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQuerySubScribe);
    if (!r) {
      err = SQL_EXEC_ERROR;
      LOG_DEBUG("SQL_EXEC_ERROR");
      break;
    }
    r = dict->GetList(L"resultvalue", &listvalue);
    if (!r || listvalue == NULL) {
      LOG_ERROR("dict->GetList()error");
      break;
    }
    while (listvalue->GetSize()) {
      base_logic::Value* result_value;
      LOG_DEBUG2("listvalue->GetSize[%d]", listvalue->GetSize());
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      dict_result_value->GetString(L"stock", out);
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

int SearchMysql::QueryStockPrice(StockPriceMap* map) {
  int err = 0;
  bool r = false;
  do {
    std::stringstream os;
    os << "call proc_QueryStockPrice();";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQueryStockPrice);
    if (!r) {
      LOG_DEBUG("QueryStockPrice sql error");
      err = SQL_EXEC_ERROR;
      break;
    }
    r = dict->GetList(L"resultvalue", &listvalue);
    if (!r || listvalue == NULL) {
      LOG_ERROR("QueryStockPrice listvalue null");
      break;
    }
    while (listvalue->GetSize()) {
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      StockPrice price;
      price.Serialization(dict_result_value);
      map->insert(StockPriceMap::value_type(price.code(), price));
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

int SearchMysql::QueryHotSubscribe(std::vector<std::string>* out) {
  int err = 0;
  bool r = false;
  do {
    std::stringstream os;
    os << "call proc_QueryHotSubscribe();";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue = NULL;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQueryHotSubscribe);
    if (!r) {
      err = SQL_EXEC_ERROR;
      LOG_ERROR("QueryHotSubscribe sql err!");
      break;
    }
    r = dict->GetList(L"resultvalue", &listvalue);
    if (!r || listvalue == NULL) {
      err = -1;
      LOG_ERROR("QueryHotSubscribe listvalue is null");
      break;
    }
    while (listvalue->GetSize()) {
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      std::string stmp;
      dict_result_value->GetString(L"code", &stmp);
      out->push_back(stmp);
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

int SearchMysql::DelSubscribe(int64 uid, std::string code, std::string codes) {
  int err = 0;
  bool r = false;
  std::stringstream os;
   os << "call proc_DelSubscribe(" << uid << ",'" << code << "','"
       << codes << "');";
   scoped_ptr<DicValue> dict(new DicValue());
   LOG_MSG(os.str().c_str());
   dict->SetString(L"sql", os.str());
   r = mysql_engine_->WriteData(0, (base_logic::Value*)(dict.get()));
   LOG_MSG("DelSubscribe");
  if (!r)
    err = SQL_EXEC_ERROR;
  return err;
}

int SearchMysql::AddSubscribe(int64 uid, std::string code, std::string codes) {
  int err = 0;
  bool r = false;
  std::stringstream os;
   os << "call proc_AddSubscribe(" << uid << ",'" << code << "','"
       << codes << "');";
   scoped_ptr<DicValue> dict(new DicValue());
   LOG_MSG(os.str().c_str());
   dict->SetString(L"sql", os.str());
   r = mysql_engine_->WriteData(0, (base_logic::Value*)(dict.get()));
   LOG_MSG("AddSubscribe");
  if (!r)
    err = SQL_EXEC_ERROR;
  return err;
}

void SearchMysql::CallQuerySubScribe(void* param, base_logic::Value* value) {
  DicValue* dict = (DicValue*)(value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*)(param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  LOG_DEBUG2("CallQuerySubScribe_count[%d]", num);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
      DicValue* info_value = new DicValue();
      if (rows[0] != NULL)
        info_value->SetString(L"stock", rows[0]);
      list->Append((base_logic::Value*)(info_value));
      LOG_DEBUG2("stock_code[%s]", rows[0]);
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*)(list));
}

void SearchMysql::CallQueryStockPrice(void* param, base_logic::Value* value) {
  DicValue* dict = (DicValue*)(value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*)(param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
      DicValue* info_value = new DicValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetReal(L"cur_price", atof(rows[1]));
      if (rows[2] != NULL)
        info_value->SetReal(L"close_price", atof(rows[2]));
      if (rows[3] != NULL)
        info_value->SetReal(L"change_percnet", atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetBigInteger(L"status_type", atol(rows[4]));
      list->Append((base_logic::Value*)(info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*)(list));
}

void SearchMysql::CallQueryAllStock(void* param, base_logic::Value* value) {
  DicValue* dict = (DicValue*)(value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*)(param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
      DicValue* info_value = new DicValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"stock_type", rows[1]);
      if (rows[2] != NULL)
        info_value->SetString(L"sefspell", rows[2]);
      if (rows[3] != NULL)
        info_value->SetString(L"name", rows[3]);
      if (rows[4] != NULL)
        info_value->SetString(L"simspell", rows[4]);
      list->Append((base_logic::Value*)(info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*)(list));
}

void SearchMysql::CallQueryHotSubscribe(void* param,
                                         base_logic::Value* value) {
  DicValue* dict = (DicValue*)(value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*)(param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
      DicValue* info_value = new DicValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      list->Append((base_logic::Value*)(info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*)(list));
}

}  // namespace news

