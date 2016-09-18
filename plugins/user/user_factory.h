//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#ifndef GEORGE_USER_FACTORY_H__
#define GEORGE_USER_FACTORY_H__

#include "user_register_mgr.h"
#include "user_login_mgr.h"
#include "user_proto_buf.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "config/config.h"
#include "net/packet_process.h"


namespace user_logic {

class UserFactory {
 public:
  UserFactory();
	virtual ~UserFactory();
 public:
	void Init();
	void Dest();
 private:
	static UserFactory*  instance_;
 public:
	static UserFactory* GetInstance();
	static void FreeInstance();

	void InitParam(config::FileConfig* config);

	void Test();

	void TimeEvent(int opcode, int time);

	//void SendError(const int socket,const int16 operator_code);
 public:
	void OnRegisterEvent(const int socket,
	                     base_logic::DictionaryValue* dict,
	                     george_logic::PacketHead* packet);

	void OnRegisterVercode(const int socket,
	                       base_logic::DictionaryValue* dict,
	                       george_logic::PacketHead* packet);

	void OnLoginEvent(const int socket,
	                  base_logic::DictionaryValue* dict,
	                  george_logic::PacketHead* packet);

	void OnLoginVercode(const int socket,
                      base_logic::DictionaryValue* dict,
                      george_logic::PacketHead* packet);

	void OnSMSLoginEvent(const int socket, base_logic::DictionaryValue* dict,
	                george_logic::PacketHead* packet);

 private:
	void SendPacket(const int socket,george_logic::PacketHead* packet,
			george_logic::AttachField* attach,
			const int16 operator_code,const int8 type);

	void SendSMSVercode(const std::string& phone,const std::string& type ,const int32 vercode);
public:
	void SendError(const int socket,george_logic::AttachField* attach,
			const int16 operator_code);

	void SendHeader(const int socket,george_logic::AttachField* attach,
			const int16 operator_code, const int8 type);


 private:
	user_logic::RegisterManager*                        register_mgr_;
	user_logic::LoginManager*                           login_mgr_;
	user_logic::UserDB*                                 user_db_;
	george_logic::json_packet::PacketProcess*           packet_json_;
	george_logic::jsonp_packet::PacketProcess*          packet_jsonp_;
};
}
#endif
