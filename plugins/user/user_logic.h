//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry

#ifndef GEORGE_USER_LOGIC_
#define GEORGE_USER_LOGIC_
#include "user_factory.h"
#include "user_packet_process.h"
#include "core/common.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"

namespace user_logic {

class UserLogic {
 public:
  UserLogic();
  virtual ~UserLogic();

 private:
  static UserLogic *instance_;

 public:
  static UserLogic *GetInstance();
  static void FreeInstance();

 public:

  bool OnUserConnect(struct server *srv, const int socket);

  bool OnUserMessage(struct server *srv, const int socket, const void *msg,
                     const int len);

  bool OnUserClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);

  bool OnBroadcastMessage(struct server *srv, const int socket, const void *msg,
                          const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 private:
  bool Init();
 private:
  user_logic::UserFactory* factory_;
  george_logic::http_packet::PacketProcess* packet_;

};

}  // namespace vip_logic

#endif

