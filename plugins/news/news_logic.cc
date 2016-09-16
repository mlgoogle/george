// Copyright (c) 2016 The news Authors. All rights reserved.
// news_logic.cc
// Created on: 2016年5月19日
// Author: Paco

#include "news/news_logic.h"

#include <assert.h>

#include "news/news_packet.h"
#include "net/comm_head.h"

#include "logic/logic_comm.h"
#include "core/common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/news/news_config.xml"
#define UPDATE_NEWS 1001
namespace news {
Newslogic* Newslogic::instance_ = NULL;

Newslogic::Newslogic() {
  if (!Init())
    assert(0);
}

Newslogic::~Newslogic() {
  NewsInterface::FreeInstance();
  if (packet_ != NULL) {
    delete packet_;
    packet_ = NULL;
  }
}

bool Newslogic::Init() {
  bool r = true;
  do {
    packet_ = new george_logic::http_packet::PacketProcess();
    news_interface_ = NewsInterface::GetInstance();
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    std::string path = DEFAULT_CONFIG_PATH;
    if (config == NULL) {
      LOG_ERROR("news config init error");
      r = false;
      break;
    }
    r = config->LoadConfig(path);
    if (!r) {
      LOG_ERROR("news config load error");
      break;
    }
    news_interface_->InitConfig(config);
    news_interface_->InitNewsMem();
//    news_interface_->Test();
  } while (0);
  return r;
}

Newslogic* Newslogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new Newslogic();
  return instance_;
}

void Newslogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool Newslogic::OnNewsConnect(struct server *srv, const int socket) {
  return true;
}

bool Newslogic::OnNewsMessage(struct server *srv, const int socket,
                              const void *msg, const int len) {
  bool r = false;
  LOG_DEBUG2("msg:[%s]", msg);
  r = packet_->UnpackPacket(socket, msg,len,george_logic::NEWS_TYPE,
                        NewsPacket::PacketPocessGet);
  if (!r) {//异常
      news_interface_->SendError(socket,NULL,0);
    return false;
  }
  return true;
}

bool Newslogic::OnNewsClose(struct server *srv, const int socket) {
  return true;
}

bool Newslogic::OnBroadcastConnect(struct server *srv, const int socket,
                                   const void *msg, const int len) {
  return true;
}

bool Newslogic::OnBroadcastMessage(struct server *srv, const int socket,
                                   const void *msg, const int len) {
  return true;
}



bool Newslogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool Newslogic::OnIniTimer(struct server *srv) {
  srv->add_time_task(srv, "news", UPDATE_NEWS, 60*5, -1);
  return true;
}



bool Newslogic::OnTimeout(struct server *srv, char *id, int opcode, int time) {
   switch (opcode) {
     case UPDATE_NEWS: {
       news_interface_->UpdateNews();
       break;
     }
  }
  return true;
}

}  // namespace bigv

