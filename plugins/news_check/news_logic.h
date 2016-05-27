// Copyright (c) 2016 The news Authors. All rights reserved.
// news_logic.h
// Created on: 2016年5月19日
// Author: Paco

#ifndef PLUGINS_NEWS_CHECK_NEWS_LOGIC_H_
#define PLUGINS_NEWS_CHECK_NEWS_LOGIC_H_

#include "core/common.h"
#include "news_check/news_interface.h"

namespace news {

class Newslogic {
 public:
  Newslogic();
  virtual ~Newslogic();

 private:
  static Newslogic  *instance_;

 public:
  static Newslogic *GetInstance();
  static void FreeInstance();

 public:
  bool OnNewsConnect(struct server *srv, const int socket);
  bool OnNewsMessage(struct server *srv, const int socket,
                     const void *msg, const int len);
  bool OnNewsClose(struct server *srv, const int socket);
  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);
  bool OnBroadcastMessage(struct server *srv, const int socket,
                          const void *msg, const int len);
  bool OnBroadcastClose(struct server *srv, const int socket);
  bool OnIniTimer(struct server *srv);
  bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 private:
  NewsInterface* news_interface_;
  bool Init();
};

}  // namespace news

#endif  // PLUGINS_NEWS_CHECK_NEWS_LOGIC_H_

