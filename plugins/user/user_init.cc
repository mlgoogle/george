//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry
#include "user_init.h"
#include "core/common.h"
#include "core/plugins.h"
#include "user_logic.h"

struct userplugin {
  char* id;
  char* name;
  char* version;
  char* provider;
};

static void *OnUserStart() {
  signal(SIGPIPE, SIG_IGN);
  struct userplugin* user = (struct userplugin*) calloc(
      1, sizeof(struct userplugin));
  user->id = "user";
  user->name = "user";
  user->version = "1.0.0";
  user->provider = "kerry";
  if (!user_logic::UserLogic::GetInstance())
    assert(0);
  return user;
}

static handler_t OnUserShutdown(struct server* srv, void* pd) {
  user_logic::UserLogic::FreeInstance();

  return HANDLER_GO_ON;
}

static handler_t OnUserConnect(struct server *srv, int fd, void *data,
                               int len) {
  user_logic::UserLogic::GetInstance()->OnUserConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUserMessage(struct server *srv, int fd, void *data,
                               int len) {
  user_logic::UserLogic::GetInstance()->OnUserMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnUserClose(struct server *srv, int fd) {
  user_logic::UserLogic::GetInstance()->OnUserClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data,
                                    int len) {
  user_logic::UserLogic::GetInstance()->OnBroadcastConnect(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
  user_logic::UserLogic::GetInstance()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data,
                                    int len) {
  user_logic::UserLogic::GetInstance()->OnBroadcastMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
  user_logic::UserLogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {
  user_logic::UserLogic::GetInstance()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int user_plugin_init(struct plugin *pl) {
  pl->init = OnUserStart;
  pl->clean_up = OnUserShutdown;
  pl->connection = OnUserConnect;
  pl->connection_close = OnUserClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnUserMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}

