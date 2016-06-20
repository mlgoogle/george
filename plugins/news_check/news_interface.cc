// Copyright (c) 2016 The george Authors. All rights reserved.
// news_interface.cc
// Created on: 2016年5月25日.
// Author: Paco.

#include "news_check/news_interface.h"

#include <sys/unistd.h>

#include "news_check/news_mysql.h"
#include "news_check/news_memery.h"
#include "news_check/news_proto_buf.h"
#include "news_check/operator_code.h"

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
  packet_json_ = NULL;
  packet_jsonp_ = NULL;
}

NewsInterface::~NewsInterface() {
  if (packet_json_) {
    delete packet_json_;
    packet_json_ = NULL;
  }
  if (packet_jsonp_) {
    delete packet_jsonp_;
    packet_jsonp_ = NULL;}
}

void NewsInterface::InitConfig(config::FileConfig* config) {
  news_mysql_ = new NewsMysql(config);
  Init();
}

void NewsInterface::Init() {
  packet_json_ = new george_logic::json_packet::PacketProcess();
  packet_jsonp_ = new george_logic::jsonp_packet::PacketProcess();
  news_memery_ = NewsMemery::GetInstance();
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

void NewsInterface::OnRelateNewsEvent(const int socket, DicValue* dic,
                                      george_logic::PacketHead* packet) {
  news::recv::RelateNews relate;
  relate.set_http_packet(dic);
  send::NewsList* news_list = new send::NewsList();
  news_memery_->QueryMemeryNews(relate.relate(), relate.key_name(),
                                relate.last_id(), relate.count(), news_list);
  news_list->set_type(3);
  news_list->set_timestamp(time(NULL));

//  packet_json_->PackPacket(socket, news_list->packet());
  SendPacket(socket, news_list, packet->attach_field(),
             RELATE_NEWS_RLY, george_logic::NEWS_TYPE);
  if (news_list) {
    delete news_list;
    news_list = NULL;
  }
}

void NewsInterface::OnStNewsEvent(const int socket, DicValue* dic,
                                  george_logic::PacketHead* packet) {
  news::recv::StNews st;
  st.set_http_packet(dic);
  send::NewsList* news_list = new send::NewsList();
  news_memery_->QueryCountNews(st.stock(), news_list);
  news_list->set_type(3);
  news_list->set_timestamp(time(NULL));
//  packet_json_->PackPacket(socket, news_list->packet());
  SendPacket(socket, news_list, packet->attach_field(),
             RELATE_COUNT_RLY, george_logic::NEWS_TYPE);
  if (news_list) {
    delete news_list;
    news_list = NULL;
  }
}

void NewsInterface::SendPacket(const int socket, george_logic::PacketHead* packet,
    george_logic::AttachField* attach,
    const int16 operator_code, const int8 type) {
  packet->set_operator_code(operator_code);
  packet->set_type(type);
  if (attach != NULL && attach->format()=="jsonp") {
    packet->attach_field()->set_callback(attach->callback());
        packet_jsonp_->PackPacket(socket,packet->packet());
  } else
    packet_json_->PackPacket(socket,packet->packet());
}

void NewsInterface::SendError(const int socket,
                              george_logic::AttachField* attach,
                              const int16 operator_code) {
  SendHeader(socket, attach, operator_code, george_logic::ERROR_TYPE);
}


void NewsInterface::SendHeader(const int socket,
                               george_logic::AttachField* attach,
                               const int16 operator_code, const int8 type) {
  george_logic::PacketHead *header  = new george_logic::PacketHead();
  SendPacket(socket, header, attach, operator_code, type);
  if (header) {
      delete header;
      header = NULL;
  }
}

}  // namespace news

