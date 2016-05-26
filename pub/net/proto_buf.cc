//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2015年9月7日 Author: kerry
#include "proto_buf.h"

namespace george_logic {

void PacketHead::set_http_head(base_logic::DictionaryValue* value) {
	bool r =  false;
	int16 packet_length = 0;
	int8 is_zip_encrypt = 0;
	int8 type = 0;
	int16 signature = 0;
	int16 operate_code = 0;
	int16 data_length = 0;
	int32 timestamp = 0;
	int64 sessionid = 0;
	int32 resverved = 0;

	int64 temp = 0;
	r = value->GetBigInteger(L"packet_length",&temp);
	if (r) {
		packet_length = temp;
		set_packet_length(packet_length);
	}

	temp = 0;
	r = value->GetBigInteger(L"is_zip_encrypt", &temp);
	if (r) {
		is_zip_encrypt =  temp;
		set_is_zip_encrypt(is_zip_encrypt);
	}

	temp = 0;
	r = value->GetBigInteger(L"type", &temp);
	if (r) {
		type = temp;
		set_type(type);
	}

	temp = 0;
	r = value->GetBigInteger(L"signature", &temp);
	if (r) {
		signature = temp;
		set_signature(signature);
	}

	temp = 0;
	r = value->GetBigInteger(L"operate_code", &temp);
	if (r) {
		operate_code = temp;
		set_operator_code(operate_code);
	}

	temp = 0;
	r = value->GetBigInteger(L"data_length", &temp);
	if (r) {
		data_length = temp;
		set_data_length(data_length);
	}

	temp = 0;
	r = value->GetBigInteger(L"timestamp", &temp);
	if (r) {
		timestamp = temp;
		set_timestamp(timestamp);
	}


	r = value->GetBigInteger(L"sessionid", &sessionid);
	if (r)
		set_sessionid(sessionid);

	temp = 0;
	r = value->GetBigInteger(L"reserved", &temp);
	if (r) {
		resverved = temp;
		set_reserved(resverved);
	}
}

}
