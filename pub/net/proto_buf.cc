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

	r = value->GetShortInteger(L"packet_length",&packet_length);
	if (r)
		set_packet_length(packet_length);

	r = value->GetCharInteger(L"is_zip_encrypt", &is_zip_encrypt);
	if (r)
		set_is_zip_encrypt(is_zip_encrypt);

	r = value->GetCharInteger(L"type", &type);
	if (r)
		set_type(type);

	r = value->GetShortInteger(L"signature", &signature);
	if (r)
		set_signature(signature);


	r = value->GetShortInteger(L"operate_code", &operate_code);
	if (r)
		set_operator_code(operate_code);

	r = value->GetShortInteger(L"data_length", &data_length);
	if (r)
		set_data_length(data_length);

	r = value->GetInteger(L"timestamp", &timestamp);
	if (r)
		set_timestamp(timestamp);

	r = value->GetBigInteger(L"sessionid", &sessionid);
	if (r)
		set_sessionid(sessionid);

	r = value->GetInteger(L"reserved", &resverved);
	if (r)
		set_reserved(resverved);
}

}
