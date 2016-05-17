//  Copyright (c) 2016-2016 The jindowin Authors. All rights reserved.
//  Created on: 2016/3/28 Author: jiaoyongqing

#include "message/message_logic.h"
#include "message/message_code.h"
#include "message/message_hy_gn.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "db/db_comm.h"
#include "db/redis_comm.h"
#include "net/message_comm.h"
#include "tools/tools.h"

namespace message_logic {

Messagelogic* Messagelogic::instance_ = NULL;

Messagelogic::Messagelogic() {
  if (!Init()) {
  }
}

Messagelogic::~Messagelogic() {
  db::DbSql::Dest();
  db::DbRedis::Dest();
}

bool Messagelogic::Init() {
  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);

  db::DbSql::Init(&config->mysql_db_list_);
  db::DbRedis::Init(&config->redis_list_);
  return true;
}

bool Messagelogic::OnMessageConnect(struct server *srv, const int socket) {
  return true;
}

bool Messagelogic::OnMessageMessage(struct server *srv, \
                                  const int socket, \
                                   const void *msg, \
                                     const int len) {
  const char* packet_stream = reinterpret_cast<const char*>(msg);
  std::string http_str(packet_stream, len);
  std::string error_str;
  int error_code = 0;

  scoped_ptr<base_logic::ValueSerializer> serializer(\
    base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP, &http_str));

  NetBase*  value = (NetBase*)\
    (serializer.get()->Deserialize(&error_code, &error_str));

  if (value == NULL) {
    error_code = STRUCT_ERROR;
    send_error(error_code, socket);
    return true;
  }

  scoped_ptr<RecvPacketBase> packet(\
    new RecvPacketBase(value));

  int32 type = packet->GetType();
  switch (type) {
    case MESSAGE_CODE:
      {
        LOG_DEBUG2("message: %s", "message_code");
        if(!tools::CheckUserIdAndToken(value,socket))
          return false;
        OnMessageCode(srv, socket, value);
      
        break;
      }
    case MESSAGE_HY_GN:
      {
        LOG_DEBUG2("message: %s", "message_hy_gn");
        if(!tools::CheckUserIdAndToken(value,socket))
          return false;
        OnMessageHyGn(srv, socket, value);
      
        break;
      }
    default:  //  end
      return false;
  }
  return true;
}

bool Messagelogic::OnMessageClose(struct server *srv, const int socket) {
  return true;
}

bool Messagelogic::OnBroadcastConnect(struct server *srv, \
                                      const int socket, \
                                       const void *msg, \
                                         const int len) {
  return true;
}

bool Messagelogic::OnBroadcastMessage(struct server *srv, \
                                      const int socket, \
                                       const void *msg, \
                                         const int len) {
  return true;
}

bool Messagelogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool Messagelogic::OnIniTimer(struct server *srv) {
  return true;
}

bool Messagelogic::OnTimeout(struct server *srv, char *id, int opcode, int time) {
  return true;
}

bool Messagelogic::OnMessageCode(struct server *srv, \
                             const int socket, \
                             NetBase* netbase, \
                              const void* msg, \
                               const int len ) {
  netcomm_recv::RecvMessageCode rcv_message_code(netbase);

  std::string str = rcv_message_code.GetJsonp();
  int json_type = str == "" ? 0 : 3;
  int error_code = rcv_message_code.GetResult();
  if (0 != error_code) {
    send_error(error_code, socket, json_type, str);
    return false;
  }

  MessageCode message_code(rcv_message_code.user_id(), \
                              rcv_message_code.code(), \
                                  str, socket);

  message_code.GetData();
  error_code = message_code.get_error_info();
  if (0 != error_code) {
    send_error(error_code, socket, json_type, str);
    return false;
  } else {
    message_code.SendMessage();
  }
  return true;
}

bool Messagelogic::OnMessageHyGn(struct server *srv, \
                                   const int socket, \
                                   NetBase* netbase, \
                                    const void* msg, \
                                     const int len ) {
  netcomm_recv::RecvMessageHyGn rcv_message_hy_gn(netbase);

  std::string str = rcv_message_hy_gn.GetJsonp();
  int json_type = str == "" ? 0 : 3;
  int error_code = rcv_message_hy_gn.GetResult();
  if (0 != error_code) {
	send_error(error_code, socket, json_type, str);
	return false;
  }

  MessageHyGn message_hy_gn(rcv_message_hy_gn.user_id(), \
                          rcv_message_hy_gn.hy_or_gn(), \
                          rcv_message_hy_gn.operate(), \
								  str, socket);
  message_hy_gn.GetData();
  error_code = message_hy_gn.get_error_info();
  if (0 != error_code) {
	send_error(error_code, socket, json_type, str);
	return false;
  } else {
	message_hy_gn.SendMessage();
  }
  return true;
}

}  //  namespace stocksvc_logic
