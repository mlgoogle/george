//  Copyright (c) 2016-2016 The jindowin Authors. All rights reserved.
//  Created on: 2016/3/28 Author: jiaoyongqing

#include <signal.h>
#include "message/message_init.h"
#include "message/message_logic.h"
#include "core/common.h"
#include "core/plugins.h"
#include "logic/logic_comm.h"


struct messageplugin {
    char* id;
    char* name;
    char* version;
    char* provider;
};


static void *OnMessageStart() {
    signal(SIGPIPE, SIG_IGN);
    struct messageplugin* search = (struct messageplugin*)calloc(1, \
                                   sizeof(struct messageplugin));

    search->id = "message";

    search->name = "message";

    search->version = "1.0.0";

    search->provider = "jiaoyongqing";

    return search;
}

static handler_t OnMessageShutdown(struct server* srv, void* pd) {
    return HANDLER_GO_ON;
}

static handler_t OnMessageConnect(struct server *srv, \
                                            int fd, \
                                        void *data, \
                                           int len) {
    message_logic::Messagelogic::GetInstance()->OnMessageConnect(srv, fd);

    return HANDLER_GO_ON;
}

static handler_t OnMessageMessage(struct server *srv, \
                                            int fd, \
                                        void *data, \
                                           int len) {
  bool r = message_logic::Messagelogic::GetInstance()->OnMessageMessage(srv, \
                                                                      fd, \
                                                                    data, \
                                                                     len);
  if (r == true)
    return HANDLER_FINISHED;
  else
    return HANDLER_GO_ON;
}

static handler_t OnMessageClose(struct server *srv, \
                                          int fd) {
    message_logic::Messagelogic::GetInstance()->OnMessageClose(srv, fd);

    return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len) {
    return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, \
                                                int fd, \
                                            void *data, \
                                               int len) {
    message_logic::Messagelogic::GetInstance()->OnBroadcastConnect(srv, \
                                                                   fd, \
                                                                 data, \
                                                                  len);
    return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
    message_logic::Messagelogic::GetInstance()->OnBroadcastClose(srv, fd);

    return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, \
                                                int fd, \
                                            void *data, \
                                               int len) {
    message_logic::Messagelogic::GetInstance()->OnBroadcastMessage(srv, \
                                                                   fd, \
                                                                 data, \
                                                                  len);
    return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
    message_logic::Messagelogic::GetInstance()->OnIniTimer(srv);

    return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {
    message_logic::Messagelogic::GetInstance()->OnTimeout(srv, id, opcode, time);

    return HANDLER_GO_ON;
}

int message_plugin_init(struct plugin *pl) {
    pl->init = OnMessageStart;

    pl->clean_up = OnMessageShutdown;

    pl->connection = OnMessageConnect;

    pl->connection_close = OnMessageClose;

    pl->connection_close_srv = OnBroadcastClose;

    pl->connection_srv = OnBroadcastConnect;

    pl->handler_init_time = OnIniTimer;

    pl->handler_read = OnMessageMessage;

    pl->handler_read_srv = OnBroadcastMessage;

    pl->handler_read_other = OnUnknow;

    pl->time_msg = OnTimeOut;

    pl->data = NULL;
    return 0;
}

