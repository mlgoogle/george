// Copyright (c) 2016 The news Authors. All rights reserved.
// news_init.cc
// Created on: 2016年5月19日
// Author: Paco

#include "news/news_init.h"

#include <assert.h>

#include "core/common.h"
#include "core/plugins.h"

#include "news/news_logic.h"
#include "net/comm_struct.h"

static void *OnNewsStart() {
  signal(SIGPIPE, SIG_IGN);
  PluginInfo* news = reinterpret_cast<PluginInfo*>
    (calloc(1, sizeof(PluginInfo)));
  news->id = "news";
  news->name = "news";
  news->version = "1.0.0";
  news->provider = "Paco";
  if (!news::Newslogic::GetInstance())
    assert(0);
  return news;
}

static handler_t OnNewsShutdown(struct server* srv, void* pd) {
  news::Newslogic::FreeInstance();
  return HANDLER_GO_ON;
}

static handler_t OnNewsConnect(struct server *srv, int fd, void *data,
                               int len) {
  news::Newslogic::GetInstance()->OnNewsConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnNewsMessage(struct server *srv, int fd, void *data,
                               int len) {
  news::Newslogic::GetInstance()->OnNewsMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnNewsClose(struct server *srv, int fd) {
  news::Newslogic::GetInstance()->OnNewsClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data,
                                    int len) {
  news::Newslogic::GetInstance()->OnBroadcastConnect(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
  news::Newslogic::GetInstance()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data,
                                    int len) {
  news::Newslogic::GetInstance()->OnBroadcastMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
  news::Newslogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {
  news::Newslogic::GetInstance()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int news_plugin_init(struct plugin *pl) {
  pl->init = OnNewsStart;
  pl->clean_up = OnNewsShutdown;
  pl->connection = OnNewsConnect;
  pl->connection_close = OnNewsClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnNewsMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}

