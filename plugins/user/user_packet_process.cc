//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry
#include "user_packet_process.h"
#include "operator_code.h"
#include "user_factory.h"
namespace user_logic {

namespace http_packet {

PacketProcess::PacketProcess(){
}


void PacketProcess::PacketPocessGet(const int socket,base_logic::DictionaryValue* dict,
			george_logic::PacketHead* packet) {
	int16 operate_code = packet->operate_code();
	user_logic::UserFactory*  factory  =
	    user_logic::UserFactory::GetInstance();

	switch (operate_code) {
	 case REGISTER_VERCODE_REQ: {
		factory->OnRegisterVercode(socket, dict, packet);
		break;
	 }
	 case REGISTER_USER_REQ : {
		 factory->OnRegisterEvent(socket, dict, packet);
		 break;
	 }
	 case LOGIN_USER_REQ : {
	   factory->OnLoginEvent(socket, dict, packet);
	   break;
	 }
	 case SMS_LOGIN_REQ : {
	   factory->OnSMSLoginEvent(socket,dict, packet);
	   break;
	 }
	 case LOGIN_VERCODE_REQ : {
	   factory->OnLoginVercode(socket, dict, packet);
	 }
	 default:
		break;
	}
}

}


}
