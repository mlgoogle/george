//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry


#include "vip_logic.h"
#include "vip_packet_process.h"
#include "basic/basictypes.h"
#include  <string>
#include <list>
#include <iostream>
#include "core/common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/vipsvc/vip_config.xml"

namespace vip_logic {

VIPLogic*
VIPLogic::instance_ = NULL;

VIPLogic::VIPLogic() {
    if (!Init())
        assert(0);
}

VIPLogic::~VIPLogic() {
	if (factory_) {
		delete factory_;
		factory_ = NULL;
	}

	if (packet_) {delete packet_; packet_ = NULL;}
}

bool VIPLogic::Init() {

	packet_ = new george_logic::http_packet::PacketProcess();
	factory_ = new vip_logic::VIPFactory();
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

VIPLogic*
VIPLogic::GetInstance() {
    if (instance_ == NULL)
        instance_ = new VIPLogic();
    return instance_;
}



void VIPLogic::FreeInstance() {
    delete instance_;
    instance_ = NULL;
}


bool VIPLogic::OnVIPConnect(struct server *srv, const int socket) {
    return true;
}



bool VIPLogic::OnVIPMessage(struct server *srv, const int socket,
        const void *msg, const int len) {
    bool r = false;

    packet_->UnpackPacket(msg,len,1,
    		vip_logic::http_packet::PacketProcess::PacketPocessGet);
    //struct PacketHead* packet = NULL;
    /*if (srv == NULL || socket < 0 || msg == NULL
            || len < PACKET_HEAD_LENGTH)
        return false;

    if (!ptl::PacketProsess::UnpackStream(msg, len, &packet)) {
        LOG_ERROR2("UnpackStream Error socket %d", socket);
        ptl::PacketProsess::HexEncode(msg, len);
        return false;
    }

    assert(packet);
    LOG_MSG("dump packet packet");
    ptl::PacketProsess::DumpPacket(packet);
    switch (packet->operate_code) {
        default:
            break;
    }
    ptl::PacketProsess::DeletePacket(msg, len, packet);*/
    return true;
}

bool VIPLogic::OnVIPClose(struct server *srv, const int socket) {
    return true;
}



bool VIPLogic::OnBroadcastConnect(struct server *srv, const int socket,
        const void *msg, const int len) {
    return true;
}

bool VIPLogic::OnBroadcastMessage(struct server *srv, const int socket,
        const void *msg, const int len) {
    return true;
}

bool VIPLogic::OnBroadcastClose(struct server *srv, const int socket) {
    return true;
}

bool VIPLogic::OnIniTimer(struct server *srv) {
    return true;
}

bool VIPLogic::OnTimeout(struct server *srv, char *id,
        int opcode, int time) {
    switch (opcode) {
     default:
        break;
    }
    return true;
}



}
