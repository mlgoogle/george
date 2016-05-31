// Copyright (c) 2016 The george Authors. All rights reserved.
// db_mysql.cc
// Created on: 2016年5月23日.
// Author: Paco.

#include "news_mysql.h"

#include <mysql/mysql.h>
#include <map>
#include <sstream>

#include "pub/net/error_comm.h"
#include "base/logic/logic_comm.h"

namespace news {

NewsMysql::NewsMysql(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

NewsMysql::~NewsMysql() {
  if(mysql_engine_){
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

int NewsMysql::QueryNews(int64 time, int64 counts, NewsMap* map) {
  int err = 0;
  bool r = false;
  do {
    std::stringstream os;
    os << "call proc_QueryNews(" << time << "," << counts << ")";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQueryNews);
    if (!r)
      err = SQL_EXEC_ERROR;
    dict->GetList(L"resultvalue", &listvalue);
    while (listvalue->GetSize()) {
      news::News news;
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      news.Serialization(dict_result_value);
      map->insert(NewsMap::value_type(news.id(), news));
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

int NewsMysql::QueryStNews(int64 now_time, int64 old_time, SimpleMap* nmap) {
  int err = 0;
  bool r = false;
  do {
    std::stringstream os;
    os << "call proc_QueryStNewsLately(" << now_time << "," << old_time << ")";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQueryStNews);
    if (!r)
      err = SQL_EXEC_ERROR;
    dict->GetList(L"resultvalue", &listvalue);
    while (listvalue->GetSize()) {
      news::SimpleNews news;
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      news.Serialization(dict_result_value);
      nmap->insert(SimpleMap::value_type(news.id(), news));
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

int NewsMysql::QueryRfNews(int64 id, int32 count, int32 type, std::string key,
                           send::NewsList* out_list) {
  int err = 0;
  bool r = false;
  do {
    std::stringstream os;
    os << "call proc_QueryRfNews(" << id << "," << count << ","
        << type << ",'" << key << "');";
    scoped_ptr<DicValue> dict(new DicValue());
    base_logic::ListValue* listvalue;
    LOG_MSG(os.str().c_str());
    dict->SetString(L"sql", os.str());
    r = mysql_engine_->ReadData(0, (base_logic::Value*)(dict.get()),
                                CallQueryNews);
    if (!r)
      err = SQL_EXEC_ERROR;
    dict->GetList(L"resultvalue", &listvalue);
    while (listvalue->GetSize()) {
      news::News news;
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      DicValue* dict_result_value = (DicValue*)(result_value);
      news.Serialization(dict_result_value);
      send::SendNews* send_news = new send::SendNews();
      send_news->set_nid(news.id());
      send_news->set_type(news.type());
      send_news->set_time(news.time());
      send_news->set_sentiment(news.sentiment());
      send_news->set_title(news.title());
      send_news->set_url(news.url());
      send_news->set_summary(news.summary());
      send_news->set_stock(news.rf_stock());
      send_news->set_industry(news.rf_industry());
      send_news->set_section(news.rf_section());
      send_news->set_from(news.from());
      out_list->set_news(send_news->get());
      delete dict_result_value;
      dict_result_value = NULL;
    }
  } while (0);
  return err;
}

void NewsMysql::CallQueryNews(void* param, base_logic::Value* value) {
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
        info_value->SetBigInteger(L"id", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetBigInteger(L"type", atoll(rows[1]));
      if (rows[2] != NULL)
        info_value->SetString(L"title", rows[2]);
      if (rows[3] != NULL)
      	info_value->SetString(L"url", rows[3]);
      if (rows[4] != NULL)
        info_value->SetBigInteger(L"time", atoll(rows[4]));
      if (rows[5] != NULL)
        info_value->SetString(L"industry", rows[5]);
      if (rows[6] != NULL)
        info_value->SetString(L"section", rows[6]);
      if (rows[7] != NULL)
      	info_value->SetString(L"stock", rows[7]);
      if (rows[8] != NULL)
        info_value->SetString(L"summary", rows[8]);
      if (rows[9] != NULL)
      	info_value->SetBigInteger(L"sentiment", atoi(rows[9]));
      if (rows[10] != NULL)
      	info_value->SetBigInteger(L"updated_time", atoll(rows[10]));
      if (rows[11] != NULL)
        info_value->SetBigInteger(L"from", atoi(rows[11]));
      list->Append((base_logic::Value*)(info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*)(list));
}

void NewsMysql::CallQueryStNews(void* param, base_logic::Value* value) {
  DicValue* dict = (DicValue*)(value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*)(param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
      DicValue* info_value = new DicValue();
      if (rows[2] != NULL) {
          if (strlen(rows[2]) < 1)
            continue;
          info_value->SetString(L"stock", rows[2]);
      } else {
          continue;
      }
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"id", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetBigInteger(L"time", atoll(rows[1]));
      if (rows[3] != NULL)
        info_value->SetBigInteger(L"sentiment", atoi(rows[3]));
      list->Append((base_logic::Value*)(info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*)(list));
}

}  // namespace news

