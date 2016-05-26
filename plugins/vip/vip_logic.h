//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry

#ifndef GEORGE_VIP_LOGIC_
#define GEORGE_VIP_LOGIC_
#include "vip_user_mgr.h"
#include "vip_factory.h"
#include "vip_packet_process.h"
#include "core/common.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"


namespace vip_logic {


class VIPLogic {
 public:
	VIPLogic();
    virtual ~VIPLogic();

 private:
    static VIPLogic    *instance_;

 public:
    static VIPLogic *GetInstance();
    static void FreeInstance();

 public:

    bool OnVIPConnect(struct server *srv, const int socket);

    bool OnVIPMessage(struct server *srv, const int socket,
            const void *msg, const int len);

    bool OnVIPClose(struct server *srv, const int socket);

    bool OnBroadcastConnect(struct server *srv, const int socket,
            const void *data, const int len);

    bool OnBroadcastMessage(struct server *srv, const int socket,
            const void *msg, const int len);

    bool OnBroadcastClose(struct server *srv, const int socket);

    bool OnIniTimer(struct server *srv);

    bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 private:
    bool OnVIPNewsEvent();
 private:
    bool Init();
 private:
    vip_logic::VIPFactory*                         factory_;
    george_logic::http_packet::PacketProcess*      packet_;

};

}  // namespace vip_logic

#endif

