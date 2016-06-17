// Copyright (c) 2016 The george Authors. All rights reserved.
// news_interface.cc
// Created on: 2016年5月25日.
// Author: Paco.

#include "search/search_interface.h"

#include <sys/unistd.h>

#include "search/search_mysql.h"
#include "search/search_memery.h"
#include "search/search_proto_buf.h"
#include "search/operator_code.h"

namespace search {
SearchInterface* SearchInterface::instance_;

SearchInterface* SearchInterface::GetInstance() {
  if (instance_ == NULL)
    instance_ = new SearchInterface();
  return instance_;
}

void SearchInterface::FreeInstance() {
  if (instance_ == NULL) {
    delete instance_;
    instance_ = NULL;
  }
}

SearchInterface::SearchInterface() {
  search_mysql_ = NULL;
  search_memery_ = NULL;
  packet_json_ = NULL;
  packet_jsonp_ = NULL;
}

SearchInterface::~SearchInterface() {
  if (packet_json_) {
    delete packet_json_;
    packet_json_ = NULL;
  }
  if (packet_jsonp_) {
    delete packet_jsonp_;
    packet_jsonp_ = NULL;}
}

void SearchInterface::InitConfig(config::FileConfig* config) {
  search_mysql_ = new SearchMysql(config);
  Init();
}

void SearchInterface::Init() {
  packet_json_ = new george_logic::json_packet::PacketProcess();
  packet_jsonp_ = new george_logic::jsonp_packet::PacketProcess();
  search_memery_ = SearchMemery::GetInstance();
}

int SearchInterface::InitMem() {
  int err = 0;
  do {
    search_memery_->InitSearchMysql(search_mysql_);
    search_memery_->InitAllStock();
  } while (0);
  return err;
}

void SearchInterface::OnSearchEvent(const int socket, DicValue* dic,
                                      george_logic::PacketHead* packet) {
  recv::SearchKey search;
  search.set_http_packet(dic);
  send::SearchList* search_list = new send::SearchList();
  search_memery_->QuerySearchStock(search.uid(), search.key_name(),
                                   search_list);
  search_list->set_type(4);
  search_list->set_timestamp(time(NULL));

//  packet_json_->PackPacket(socket, news_list->packet());
  SendPacket(socket, search_list, packet->attach_field(),
             SEARCH_RLY, george_logic::SEARCH_TYPE);
  if (search_list) {
    delete search_list;
    search_list = NULL;
  }
}

void SearchInterface::OnSubscribeEvent(const int socket, DicValue* dic,
                                  george_logic::PacketHead* packet) {
  recv::BaseRecv base;
  base.set_http_packet(dic);
  send::SearchList* search_list = new send::SearchList();
  search_memery_->QueryUserSubscribe(base.uid(), search_list);
  search_list->set_type(4);
  search_list->set_timestamp(time(NULL));
//  packet_json_->PackPacket(socket, news_list->packet());
  SendPacket(socket, search_list, packet->attach_field(),
             QUERY_SUBSCRIBE_RLY, george_logic::SEARCH_TYPE);
  if (search_list) {
    delete search_list;
    search_list = NULL;
  }
}

void SearchInterface::OnHotSubscribeEvent(const int socket, DicValue* dic,
                          george_logic::PacketHead* packet) {
  recv::BaseRecv base;
  base.set_http_packet(dic);
  send::SearchList* search_list = new send::SearchList();
  search_memery_->QueryHotSubscribe(base.uid(), search_list);
  search_list->set_type(4);
  search_list->set_timestamp(time(NULL));

//  packet_json_->PackPacket(socket, news_list->packet());
  SendPacket(socket, search_list, packet->attach_field(),
             HOT_SUBSCRIBE_RLY, george_logic::SEARCH_TYPE);
  if (search_list) {
    delete search_list;
    search_list = NULL;
  }
}

void SearchInterface::OnAlterSubscribeEvent(const int socket, DicValue* dic,
                           george_logic::PacketHead* packet) {
  recv::AlterSubscribe alter;
  alter.set_http_packet(dic);
  search_memery_->AlterSubscribe(alter.uid(), alter.alter_type(),
                                 alter.stock());

  SendHeader(socket, packet->attach_field(),ALTER_SUBSCRIBE_RLY,
      george_logic::SEARCH_TYPE);
//  search_list->set_type(4);
//  search_list->set_timestamp(time(NULL));
//
////  packet_json_->PackPacket(socket, news_list->packet());
//  SendPacket(socket, search_list, packet->attach_field(),
//             HOT_SUBSCRIBE_RLY, george_logic::SEARCH_TYPE);
//  if (search_list) {
//    delete search_list;
//    search_list = NULL;
//  }
}

void SearchInterface::SendPacket(const int socket, george_logic::PacketHead* packet,
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

void SearchInterface::SendError(const int socket,
                              george_logic::AttachField* attach,
                              const int16 operator_code) {
  SendHeader(socket, attach, operator_code, george_logic::ERROR_TYPE);
}


void SearchInterface::SendHeader(const int socket,
                               george_logic::AttachField* attach,
                               const int16 operator_code, const int8 type) {
  george_logic::PacketHead *header  = new george_logic::PacketHead();
  SendPacket(socket, header, attach, operator_code, type);
  if (header) {
      delete header;
      header = NULL;
  }
}

void SearchInterface::UpdateStockPrice() {
  search_memery_->UpdateStockPrice();
}

}  // namespace search

