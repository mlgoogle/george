//  Copyright (c) 2016-2016 The jindowin Authors. All rights reserved.
//  Created on: 2016/3/28 Author: jiaoyongqing
#ifndef _HOME_RUNNER_JINDOWIN_PLUGINS_SEARCH_SEARCH_LOGIC_H_
#define _HOME_RUNNER_JINDOWIN_PLUGINS_SEARCH_SEARCH_LOGIC_H_

#include "net/operator_code.h"
#include "net/error_comm.h"
#include "core/common.h"
#include "search/search_interface.h"
#include "pub/net/packet_process.h"

namespace search {
class SearchLogic {
 public:
  virtual ~SearchLogic();

 private:
  SearchLogic();

  static SearchLogic *instance_;

 public:
  static SearchLogic *GetInstance();
  static void FreeInstance();

 public:
  bool OnSearchConnect(struct server *srv, const int socket);

  bool OnSearchMessage(struct server *srv, const int socket, const void *msg,
                       const int len);

  bool OnSearchClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);

  bool OnBroadcastMessage(struct server *srv, const int socket, const void *msg,
                          const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 private:
  bool Init();
  SearchInterface* search_interface_;
  george_logic::http_packet::PacketProcess* packet_;
};

}  //  namespace search_logic

#endif  //  _HOME_RUNNER_JINDOWIN_PLUGINS_SEARCH_SEARCH_LOGIC_H_

