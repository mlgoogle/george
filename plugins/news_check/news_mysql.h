// Copyright (c) 2016 The george Authors. All rights reserved.
// db_mysql.h
// Created on: 2016年5月23日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_MYSQL_H_
#define PLUGINS_NEWS_CHECK_NEWS_MYSQL_H_

#include <vector>
#include <list>
#include <string>

#include "news_check/news_base.h"
#include "public/config/config.h"
#include "pub/storage/data_engine.h"

namespace news {

class NewsMysql {
 public:
  NewsMysql(config::FileConfig* config);
  ~NewsMysql();

 public:
  int QueryNews(int64 time, int64 counts, NewsMap* map);
  static void CallQueryNews(void* param, base_logic::Value* value);
 private:
  base_logic::DataEngine* mysql_engine_;
};

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_NEWS_MYSQL_H_
