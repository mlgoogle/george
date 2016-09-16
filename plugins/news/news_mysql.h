// Copyright (c) 2016 The george Authors. All rights reserved.
// db_mysql.h
// Created on: 2016年5月23日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_MYSQL_H_
#define PLUGINS_NEWS_CHECK_NEWS_MYSQL_H_

#include <vector>
#include <list>
#include <string>

#include "news/news_base.h"
#include "news/news_proto_buf.h"
#include "config/config.h"
#include "storage/data_engine.h"

namespace news {

class NewsMysql {
 public:
  NewsMysql(config::FileConfig* config);
  ~NewsMysql();

 public:
  int QueryNewsDetails(int64 nid, News* news);
  int QueryStNews(int64 now_time, int64 old_time, SimpleMap* nmap);
  int QueryNews(int64 time, int64 counts, NewsMap* map);
  int QueryRfNews(int64 id, int32 count, int32 type, std::string key,
                  send::NewsList* out_list);

//  static void CallQueryNewsDetails(void* param, base_logic::Value* value);
  static void CallQueryStNews(void* param, base_logic::Value* value);
  static void CallQueryNews(void* param, base_logic::Value* value);
 private:
  base_logic::DataEngine* mysql_engine_;
};

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_NEWS_MYSQL_H_
