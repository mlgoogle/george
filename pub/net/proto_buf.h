//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2015年9月7日 Author: kerry


#ifndef GEORGE_NET_PROTO_BUF_H_
#define GEORGE_NET_PROTO_BUF_H_


#include "basic/basictypes.h"
#include "logic/base_values.h"

namespace george_logic {
class PacketHead {

 public:
	PacketHead() {
		packet_length_ = is_zip_encrypt_ = type_ = signature_ = NULL;
		operate_code_ = data_length_ = timestamp_ = session_id_ = NULL;
		reserved_ = NULL;
		head_value_ = body_value_ = packet_value_ = NULL;
	}
	~PacketHead() {
		if (packet_length_) {delete packet_length_; packet_length_ = NULL;}
		if (is_zip_encrypt_) {delete is_zip_encrypt_; is_zip_encrypt_ =NULL;}
		if (type_) {delete type_;type_ = NULL;}
		if (signature_) {delete signature_; signature_ = NULL;}
		if (operate_code_) {delete operate_code_; operate_code_ = NULL;}
		if (data_length_) {delete data_length_; data_length_ = NULL;}
		if (timestamp_) {delete timestamp_; timestamp_ = NULL;}
		if (session_id_) {delete session_id_; session_id_ = NULL;}
		if (reserved_) {delete reserved_; reserved_ = NULL;}
		if (head_value_) {delete head_value_; head_value_ = NULL;}
	}

	//http结构不能多结构话，故需要特殊处理
	void set_http_head(base_logic::DictionaryValue* value);

	virtual void set_http_packet(base_logic::DictionaryValue* value){}



	void set_packet_length(const int16 packet_length) {packet_length_ =
    	new base_logic::FundamentalValue(packet_length);}

    void set_is_zip_encrypt(const int8 is_zip_encrypt) {is_zip_encrypt_ =
    	new base_logic::FundamentalValue(is_zip_encrypt);}

    void set_type(const int8 type) {type_ =
    	new base_logic::FundamentalValue(type);}

    void set_signature(const int16 signature) {signature_ =
    	new base_logic::FundamentalValue(signature);}

    void set_operator_code(const int16 operate_code) {operate_code_ =
    	new base_logic::FundamentalValue(operate_code);}

    void set_data_length(const int16 data_length) {data_length_ =
    	new base_logic::FundamentalValue(data_length);}

    void set_timestamp(const int32 timestamp) {timestamp_ =
    	new base_logic::FundamentalValue(timestamp);}

    void set_sessionid(const int64 sessionid) {session_id_ =
    	new base_logic::FundamentalValue(sessionid);}

    void set_reserved(const int32 reserved) {reserved_ =
    	new base_logic::FundamentalValue(reserved);}

    const int8 type() const {
    	int8 type = 0;
    	type_->GetAsCharInteger(&type);
    	return type;
    }

    const int16 operate_code() const {
    	int16 operate_code = 0;
    	operate_code_->GetAsShortInteger(&operate_code);
    	return operate_code;
    }

 public:
	base_logic::DictionaryValue* head() {
		head_value_ = new base_logic::DictionaryValue();
		if (packet_length_ != NULL)
			head_value_->Set(L"packet_length",packet_length_);
		if (is_zip_encrypt_ != NULL)
			head_value_->Set(L"is_zip_encrypt_",is_zip_encrypt_);
		//if (type_ != NULL)
			//head_value_->Set(L"type",type_);
		if (signature_ != NULL)
			head_value_->Set(L"signature",signature_);
		if (operate_code_ != NULL)
			head_value_->Set(L"operate_code",operate_code_);
		if (data_length_ != NULL)
			head_value_->Set(L"data_length",data_length_);
		if (timestamp_ == NULL)
			head_value_->SetBigInteger(L"timestamp",time(NULL));
		else
			head_value_->Set(L"timestamp",timestamp_);
		if (session_id_ != NULL)
			head_value_->Set(L"session_id",session_id_);
		if (reserved_ != NULL)
			head_value_->Set(L"reserved",reserved_);
		return head_value_;
	}

	virtual base_logic::DictionaryValue* body(){
		return NULL;
	}

	base_logic::DictionaryValue* packet() {
		packet_value_ = new base_logic::DictionaryValue();
		if (this->head()) packet_value_->Set(L"head",this->head_value_);
		if (this->body()) packet_value_->Set(L"body",this->body_value_);
		return packet_value_;
	}

 private:
	base_logic::FundamentalValue*  packet_length_;
	base_logic::FundamentalValue*  is_zip_encrypt_;
	base_logic::FundamentalValue*  type_;
	base_logic::FundamentalValue*  signature_;
	base_logic::FundamentalValue*  operate_code_;
	base_logic::FundamentalValue*  data_length_;
	base_logic::FundamentalValue*  timestamp_;
	base_logic::FundamentalValue*  session_id_;
	base_logic::FundamentalValue*  reserved_;
	base_logic::DictionaryValue*   head_value_;
 protected:
	base_logic::DictionaryValue*   body_value_;
	base_logic::DictionaryValue*   packet_value_;
};


/*
class PacketProsess {
 public:
	static bool UnPacketStream(const void* packet_stream,int32 len);
};*/
}
//  packet_length 长度为原始数据长度
struct PacketHead{
    int16 packet_length;
    int8  is_zip_encrypt;
    int8  type;
    int16 signature;
    int16 operate_code;
    int16 data_length;
    int32 timestamp;
    int64 session_id;
    int32 reserved;
};


#define PACKET_HEAD_LENGTH (sizeof(int16) * 4 + sizeof(int8) * 2 + \
    sizeof(int32) * 2 + sizeof(int64))


#endif
