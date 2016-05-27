// Copyright (c) 2016 The george Authors. All rights reserved.
// new_interface.h
// Created on: 2016年5月25日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_INTERFACE_H_
#define PLUGINS_NEWS_CHECK_NEWS_INTERFACE_H_

#include "public/config/config.h"

#include "news_check/news_mysql.h"
namespace news {

class NewsInterface {

 public:
  NewsInterface();
  ~NewsInterface();

 public:
  void InitConfig(config::FileConfig* config);
  int InitNewsMem();
  void UpdateNews();
  void Test();
 private:
  NewsMysql* news_mysql_;
};

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_NEWS_INTERFACE_H_
