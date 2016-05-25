//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry
#include "packet_process.h"
#include "proto_buf.h"
#include "basic/scoped_ptr.h"

namespace george_logic {

namespace http_packet {

PacketProcess::PacketProcess() {
	serializer_ = base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP);
}

PacketProcess::~PacketProcess() {
	if (serializer_){delete serializer_; serializer_ = 	NULL;}
}

void PacketProcess::UnpackPacket(const void *msg, const int len, const int8 type,
		void (*packet_process)(base_logic::DictionaryValue*,
				george_logic::PacketHead*)) {

    std::string error_str;
    int error_code = 0;
	std::string packet_stream((const char*)msg,len);
	base_logic::Value* value = serializer_->Deserialize(&packet_stream,
			&error_code, &error_str);

	george_logic::PacketHead* packet = NULL;

	base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)value;
	//通过type判断数据什么packet
	if (packet->type() == type)
		packet_process(dict,packet);

/*	packet->set_http_head(dict);
	switch (packet->type()) {
	  case 1:{

		  break;
	  }

	  default:
		break;
	}*/
	//*dict = (base_logic::DictionaryValue*)value;
}



}


}
