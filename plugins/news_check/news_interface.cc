// Copyright (c) 2016 The george Authors. All rights reserved.
// news_interface.cc
// Created on: 2016年5月25日.
// Author: Paco.

#include "news_check/news_interface.h"

#include <sys/unistd.h>

#include "news_check/news_mysql.h"
#include "news_check/news_memery.h"
#include "news_check/news_proto_buf.h"

namespace news {
NewsInterface* NewsInterface::instance_;

NewsInterface* NewsInterface::GetInstance() {
  if (instance_ == NULL)
    instance_ = new NewsInterface();
  return instance_;
}

void NewsInterface::FreeInstance() {
  if (instance_ == NULL) {
    delete instance_;
    instance_ = NULL;
  }
}

NewsInterface::NewsInterface() {
  news_mysql_ = NULL;
  news_memery_ = NULL;
  packet_ = NULL;
}

NewsInterface::~NewsInterface() {

}

void NewsInterface::InitConfig(config::FileConfig* config) {
  news_mysql_ = new NewsMysql(config);
  Init();
}

void NewsInterface::Init() {
  packet_ = new george_logic::json_packet::PacketProcess();
  news_memery_ = NewsMemery::GetInstance();
  news_memery_->GetDaysMilliSecond(0);
}

int NewsInterface::InitNewsMem() {
  int err = 0;
  do {
    news_memery_->InitNewMysql(news_mysql_);
    UpdateNews();
  } while (0);
  return err;
}

void NewsInterface::UpdateNews() {
  NewsMemery::GetInstance()->UpdateCacheNews();
// Test();
}

void NewsInterface::Test() {
  NewsMemery::GetInstance()->GetRfNews("q", 1);
}

void NewsInterface::OnRelateNewsEvent(const int socket, DicValue* dic) {
  news::recv::RelateNews relate;
  relate.set_http_packet(dic);
  send::NewsList* news_list = new send::NewsList();
  news_memery_->QueryMemeryNews(relate.relate(), relate.key_name(),
                                relate.last_id(), relate.count(), news_list);
  news_list->set_type(3);
  news_list->set_timestamp(time(NULL));

  packet_->PackPacket(socket, news_list->packet());

}

void NewsInterface::OnStNewsEvent(const int socket, DicValue* dic) {
  news::recv::StNews st;
  st.set_http_packet(dic);
  send::NewsList* news_list = new send::NewsList();
  news_memery_->QueryCountNews(st.stock(), news_list);
  news_list->set_type(3);
  news_list->set_timestamp(time(NULL));
  packet_->PackPacket(socket, news_list->packet());
}



}  // namespace news

