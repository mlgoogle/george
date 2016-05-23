//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry
#include "vip_init.h"
#include "core/common.h"
#include "core/plugins.h"
#include "vip_logic.h"


struct vipplugin{
    char* id;
    char* name;
    char* version;
    char* provider;
};


static void *OnVIPStart() {
    signal(SIGPIPE, SIG_IGN);
    struct vipplugin* vip = (struct vipplugin*)calloc(1,
            sizeof(struct vipplugin));
    vip->id = "vip";
    vip->name = "vip";
    vip->version = "1.0.0";
    vip->provider = "kerry";
    if (!vip_logic::VIPLogic::GetInstance())
        assert(0);
    return vip;
}

static handler_t OnVIPShutdown(struct server* srv, void* pd) {
	vip_logic::VIPLogic::FreeInstance();

    return HANDLER_GO_ON;
}

static handler_t OnVIPConnect(struct server *srv, int fd, void *data,
        int len) {
	vip_logic::VIPLogic::GetInstance()->OnVIPConnect(srv, fd);
    return HANDLER_GO_ON;
}

static handler_t OnVIPMessage(struct server *srv, int fd, void *data,
        int len) {
	vip_logic::VIPLogic::GetInstance()->OnVIPMessage(srv,
            fd, data, len);
    return HANDLER_GO_ON;
}

static handler_t OnVIPClose(struct server *srv, int fd) {
	vip_logic::VIPLogic::GetInstance()->OnVIPClose(srv, fd);
    return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data,
        int len) {
    return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, int fd,
        void *data, int len) {
	vip_logic::VIPLogic::GetInstance()->OnBroadcastConnect(
            srv, fd, data, len);
    return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
	vip_logic::VIPLogic::GetInstance()->OnBroadcastClose(srv, fd);
    return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data,
        int len) {
	vip_logic::VIPLogic::GetInstance()->OnBroadcastMessage(srv,
            fd, data, len);
    return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
	vip_logic::VIPLogic::GetInstance()->OnIniTimer(srv);
    return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {
	vip_logic::VIPLogic::GetInstance()->OnTimeout(srv,
            id, opcode, time);
    return HANDLER_GO_ON;
}


int vip_plugin_init(struct plugin *pl) {
    pl->init = OnVIPStart;
    pl->clean_up = OnVIPShutdown;
    pl->connection = OnVIPConnect;
    pl->connection_close = OnVIPClose;
    pl->connection_close_srv = OnBroadcastClose;
    pl->connection_srv = OnBroadcastConnect;
    pl->handler_init_time = OnIniTimer;
    pl->handler_read = OnVIPMessage;
    pl->handler_read_srv = OnBroadcastMessage;
    pl->handler_read_other = OnUnknow;
    pl->time_msg = OnTimeOut;
    pl->data = NULL;
    return 0;
}

