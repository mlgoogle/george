
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "net/user_comm_head.h"
#include "net/error_comm.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "logic/base_values.h"
#include "db/db_comm.h"
#include "base/logic/logic_comm.h"
#include "core/common.h"

#include "user/login.h"
#include "user/user_logic.h"
#include "user/register.h"
#include "user/kafka_accessor.h"
#include "tools/tools.h"
#include "user/user_manager.h"

namespace user_svc_logic{

Userlogic* Userlogic::instance_=NULL;

Userlogic::Userlogic(){
 if(!Init())
    assert(0);
}

Userlogic::~Userlogic(){
  db::DbSql::Dest();
  user::KafkaAccessor::Release();
}

bool Userlogic::Init(){
  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);

  LOG_DEBUG2("user  size: %d", config->redis_list_.size());
  db::DbSql::Init(&config->mysql_db_list_);
	    // local kafka config
  LOG_DEBUG2("KafkaConfig]host:%s,port:%d,topic:%s", \
      config->kafka_conf_list_.front().host().c_str(),
      config->kafka_conf_list_.front().port(),
      config->kafka_conf_list_.front().source().c_str());

  if (0 <= user::KafkaAccessor::Init(0,
    config->kafka_conf_list_.front().host(),
    config->kafka_conf_list_.front().port(),
    config->kafka_conf_list_.front().source().c_str())) {
    LOG_DEBUG("user Kafka init successful...");
  }
//  InitVerifyTemp();
  return true;
}

bool Userlogic::InitVerifyTemp() {
  std::ifstream in(VERIFY_TEMP_PATH, std::ios::in | std::ios::binary);
  if (in) {
    in.seekg(0, std::ios::end);
    email_html_.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&email_html_[0], email_html_.size());
    in.close();
  }
  return true;
}

Userlogic* Userlogic::GetInstance(){
  if(instance_==NULL)
      instance_ = new Userlogic();
  return instance_;
}

bool Userlogic::OnUserConnect(struct server *srv,const int socket){
    return true;
}

bool Userlogic::OnUserMessage(struct server *srv,
			      const int socket,
			      const void *msg,
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
  LOG_DEBUG2("-----type-------%d", type);
  switch (type)
  {
    case USER_LOGIN:
	OnLogin(srv, socket, value);
      break;
    case USER_REGISTER:
	OnRegister(srv, socket, value);
      break;
    case VERIFY_EMAIL:
      OnVerifyEmail(srv, socket, value);
      break;
    case USER_ATTR:
      OnUserAttr(srv, socket, value);
    default:  //  end
      return false;
  }

  return true;
}

bool Userlogic::OnUserClose(struct server *srv,const int socket){

    return true;
}



bool Userlogic::OnBroadcastConnect(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}

bool Userlogic::OnBroadcastMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}



bool Userlogic::OnBroadcastClose(struct server *srv, const int socket){

    return true;
}

bool Userlogic::OnIniTimer(struct server *srv){

    return true;
}



bool Userlogic::OnTimeout(struct server *srv, char *id, int opcode, int time){

    return true;
}

bool Userlogic::OnLogin(struct server *srv, const int socket, NetBase* netbase,
		const void* msg, const int len)
{
  netcomm_recv::RecvLogin recv_login(netbase);
  std::string jsonp_call = recv_login.GetJsonp();
  int json_type = jsonp_call == "" ? 0 : 3;
  int error_code = recv_login.GetResult();
  if (error_code != 0) {
	  send_error(error_code, socket, json_type, jsonp_call);
	  return false;
  }

  login::Login login(recv_login.platform_id(),
			  recv_login.user_name(),
			  recv_login.password(),
			  recv_login.user_type(),\
			  jsonp_call, socket);
  login.GetData();
  error_code = login.get_error_info();
  if (error_code != 0) {
    send_error(error_code, socket, json_type, jsonp_call);
    return false;
  }
  login.SendMessage();

  return true;
}


bool Userlogic::OnRegister(struct server *srv, const int socket, NetBase* netbase, const void* msg, const int len)
{
  netcomm_recv::RecvRegister recv_register(netbase);
  std::string jsonp_call = recv_register.GetJsonp();
  int json_type = jsonp_call == "" ? 0 : 3;
  int error_code = recv_register.GetResult();
  if (error_code != 0) {
    send_error(error_code, socket, json_type, jsonp_call);
    return false;
  }

  user_register::Register sign(recv_register.platform_id(),
      recv_register.user_name(),
      recv_register.password(),
      jsonp_call, socket);

  error_code = sign.CheckUser();
  LOG_DEBUG2("--------error_code:%d",error_code);
  if (error_code != 0){
    send_error(error_code, socket, json_type, jsonp_call);
    return false;
  }
  sign.SendMessage();
}

bool Userlogic::OnVerifyEmail(struct server *srv,
	const int socket, NetBase* netbase,
	const void* msg, const int len) {
  int32 error = 0;
  do {
    netcomm_recv::RecvVerifyEmail recv_active_email(netbase);
    error = recv_active_email.GetResult();
    if (error != 0)
      break;

    std::string email = recv_active_email.get_email();
    std::string param = recv_active_email.get_param();
    param = tools::TeaDecode(param);
    std::vector<std::string> result = tools::Split(param, ";");
    int64 time = tools::CurrentTime();
    if(result.size() < 4 ||
	email != result[0] ) {
      error = ACTIVE_URL_ERROR;
      break;
    }
    user_register::Register sign(atoi(result[2].c_str()),
				   result[0],
				   result[1],
          "", socket);
    if((time-atol(result[3].c_str())) > 15*60) {
      error = ACTIVE_URL_INVALID;
      sign.SendActivrEmail();
      break;
    }
    UserInfo* user = UserManager::GetInstance()->FindRegist(result[0]);
    if (user == NULL) {
      error = ACTIVE_URL_ERROR;
      break;
    } else {
      UserManager::GetInstance()->DelRegist(user->email);
      sign.InsertUser();
      login::Login login(atoi(result[2].c_str()),
			 result[0],
			 result[1],
      			 1,
      			 "", socket);
      login.GetData();
      login.SendMessage();
    }
  } while(0);
  if (error < 0)
    send_error(error, socket);
  return true;
}

bool Userlogic::OnUserAttr(struct server *srv,
	const int socket, NetBase* netbase,
	const void* msg, const int len) {
  int error = 0;
  int status = 0;
  do {
    netcomm_recv::RecvVerifyEmail recv_active_email(netbase);
    std::string email = recv_active_email.get_email();
    if (email.length() < 5) {
	error = STRUCT_ERROR;
	break;
    }
    db::DbSql sql;
    std::string username = "";
    username = sql.GetUserNameInfo(email);
    if (username == email)
      status = 1;
  } while(0);
  if (error < 0) {
    send_error(error, socket);
  } else {
    SendPacketBase base;
    base.set_status(status);
    base.set_msg("");
    send_message(socket, &base);
  }

  return true;

}

}

