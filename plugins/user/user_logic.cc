//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry

#include "user_logic.h"
#include "user_packet_process.h"
#include "net/comm_head.h"
#include "basic/basictypes.h"
#include  <string>
#include <list>
#include <iostream>
#include "core/common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/usersvc/user_config.xml"

namespace user_logic {

UserLogic* UserLogic::instance_ = NULL;

UserLogic::UserLogic() {
  if (!Init())
    assert(0);
}

UserLogic::~UserLogic() {

  user_logic::UserFactory::GetInstance()->Dest();

  if (factory_) {
    delete factory_;
    factory_ = NULL;
  }

  if (packet_) {
    delete packet_;
    packet_ = NULL;
  }
}

bool UserLogic::Init() {

  packet_ = new george_logic::http_packet::PacketProcess();
  factory_ = user_logic::UserFactory::GetInstance();
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  std::string path = DEFAULT_CONFIG_PATH;
  if (config == NULL)
    return false;
  bool r = config->LoadConfig(path);
  if (!r)
    return r;
  factory_->InitParam(config);

  factory_->Test();
  return true;
}

UserLogic*
UserLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new UserLogic();
  return instance_;
}

void UserLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool UserLogic::OnUserConnect(struct server *srv, const int socket) {
  return true;
}

bool UserLogic::OnUserMessage(struct server *srv, const int socket,
                              const void *msg, const int len) {
  bool r = false;

  if (srv == NULL || socket < 0 || msg == NULL || len < 0)
    return false;

  r = packet_->UnpackPacket(
      socket, msg, len, george_logic::USER_TYPE,
      user_logic::http_packet::PacketProcess::PacketPocessGet);
  if (!r) {  //异常
    factory_->SendError(socket, NULL, 0);
    return false;
  }

  return true;
}

bool UserLogic::OnUserClose(struct server *srv, const int socket) {
  return true;
}

bool UserLogic::OnBroadcastConnect(struct server *srv, const int socket,
                                   const void *msg, const int len) {
  return true;
}

bool UserLogic::OnBroadcastMessage(struct server *srv, const int socket,
                                   const void *msg, const int len) {
  return true;
}

bool UserLogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool UserLogic::OnIniTimer(struct server *srv) {
  if (srv->add_time_task != NULL) {
    //srv->add_time_task(srv, "user", TIME_UPDATE_ACRTICLE, 100, -1);
  }
  return true;
}

bool UserLogic::OnTimeout(struct server *srv, char *id, int opcode, int time) {
  //factory_->TimeEvent(opcode, time);
  return true;
}

}
