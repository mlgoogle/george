// Copyright (c) 2016 The george Authors. All rights reserved.
// news_interface.cc
// Created on: 2016年5月25日.
// Author: Paco.

#include "news_check/news_interface.h"

#include "news_check/news_mysql.h"
#include "news_check/news_memery.h"
#include <sys/unistd.h>
namespace news {

NewsInterface::NewsInterface() {
  news_mysql_ = NULL;
}

NewsInterface::~NewsInterface() {

}

void NewsInterface::InitConfig(config::FileConfig* config) {
  news_mysql_ = new NewsMysql(config);
}

int NewsInterface::InitNewsMem() {
  int err = 0;
  do {
    NewsMemery::GetInstance()->InitNewMysql(news_mysql_);
    UpdateNews();
  } while (0);
  return err;
}

void NewsInterface::UpdateNews() {
  NewsMemery::GetInstance()->UpdateCacheNews();
}

void NewsInterface::Test() {
  NewsMemery::GetInstance()->GetRfNews("q", 1);

  NewsMemery::GetInstance()->UpdateCacheNews();
  NewsMemery::GetInstance()->GetRfNews("q", 1);
}



}  // namespace news

