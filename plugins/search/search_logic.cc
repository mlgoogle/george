//  Copyright (c) 2016-2016 The jindowin Authors. All rights reserved.
//  Created on: 2016/3/28 Author: jiaoyongqing

#include "search/search_logic.h"

#include <assert.h>

#include "search/search_packet.h"
#include "net/comm_head.h"

#include "logic/logic_comm.h"
#include "core/common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/search/search_config.xml"
#define UPDATE_STOCK_PRICE  120
namespace search {
SearchLogic* SearchLogic::instance_ = NULL;

SearchLogic::SearchLogic() {
  if (!Init()) {
    assert(0);
  }
}

SearchLogic::~SearchLogic() {
  SearchInterface::FreeInstance();
  if (packet_ != NULL) {
    delete packet_;
    packet_ = NULL;
  }
}

SearchLogic* SearchLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new SearchLogic();
  return instance_;
}

bool SearchLogic::Init() {
  bool r = true;
  do {
    packet_ = new george_logic::http_packet::PacketProcess();
    search_interface_ = SearchInterface::GetInstance();
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
    search_interface_->InitConfig(config);
    search_interface_->InitMem();
    search_interface_->UpdateStockPrice();
  } while (0);
  return r;
}

bool SearchLogic::OnSearchConnect(struct server *srv, const int socket) {
  return true;
}

bool SearchLogic::OnSearchMessage(struct server *srv, const int socket,
                                  const void *msg, const int len) {
  bool r = false;
  LOG_DEBUG2("msg:[%s]", msg);
  r = packet_->UnpackPacket(socket, msg,len,george_logic::SEARCH_TYPE,
                        SearchPacket::PacketPocessGet);
  if (!r) {//异常
      search_interface_->SendError(socket,NULL,0);
    return false;
  }
  return true;
}

bool SearchLogic::OnSearchClose(struct server *srv, const int socket) {
  return true;
}

bool SearchLogic::OnBroadcastConnect(struct server *srv,  const int socket,
                                     const void *msg, const int len) {
  return true;
}

bool SearchLogic::OnBroadcastMessage(struct server *srv, const int socket,
                                     const void *msg, const int len) {
  return true;
}

bool SearchLogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool SearchLogic::OnIniTimer(struct server *srv) {
  srv->add_time_task(srv, "news", UPDATE_STOCK_PRICE, 20, -1);
  return true;
}

bool SearchLogic::OnTimeout(struct server *srv, char *id, int opcode,
                            int time) {
   switch (opcode) {
     case UPDATE_STOCK_PRICE: {
       search_interface_->UpdateStockPrice();
       break;
     }
  }
  return true;
}

}  //  namespace search_logic
