//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#include "user_factory.h"
#include "user_proto_buf.h"
#include "operator_code.h"
#include "basic/template.h"
#include "basic/basic_util.h"
#include "logic/logic_comm.h"
#include "http/http_api.h"
#include "check/token.h"
//#include "tools/tools.h"
#include <list>

namespace user_logic {

UserFactory* UserFactory::instance_ = NULL;

UserFactory*
UserFactory::GetInstance() {
  if (instance_ == NULL)
    instance_ = new UserFactory();
  return instance_;
}

void UserFactory::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

UserFactory::UserFactory() {
  Init();
}

UserFactory::~UserFactory() {
  if (packet_json_) {
    delete packet_json_;
    packet_json_ = NULL;
  }
  if (packet_jsonp_) {
    delete packet_jsonp_;
    packet_jsonp_ = NULL;
  }
  if (user_db_) {
    delete user_db_;
    user_db_ = NULL;
  }

}

void UserFactory::Init() {
  register_mgr_ = user_logic::RegisterEngine::GetRegisterManager();
  login_mgr_ = user_logic::LoginEngine::GetLoginManager();
  packet_json_ = new george_logic::json_packet::PacketProcess();
  packet_jsonp_ = new george_logic::jsonp_packet::PacketProcess();
}

void UserFactory::InitParam(config::FileConfig* config) {
  user_db_ = new user_logic::UserDB(config);
  register_mgr_->Init(user_db_);
  login_mgr_->Init(user_db_);

}
void UserFactory::Dest() {
  user_logic::RegisterEngine::FreeRegisterManager();
  user_logic::RegisterEngine::FreeRgisterEngine();
  user_logic::LoginEngine::FreeLoginManager();
  user_logic::LoginEngine::FreeLoginEngine();
}

void UserFactory::Test() {
}

void UserFactory::TimeEvent(int opcode, int time) {
}

void UserFactory::OnRegisterVercode(const int socket,
                                    base_logic::DictionaryValue* dict,
                                    george_logic::PacketHead* packet) {
  user_logic::net_request::UserGetVerCode* vercode =
      new user_logic::net_request::UserGetVerCode();
  int32 code = 0;
  vercode->set_http_packet(dict);
  bool r = register_mgr_->CreateVerCode(vercode->phone(), code);
  //发送
  user_logic::net_reply::RegisterVercode* t_vercode =
      new user_logic::net_reply::RegisterVercode();
  t_vercode->set_vercode(code);
  if (r)
    t_vercode->set_status(1);
  else
    t_vercode->set_status(0);
  SendPacket(socket, t_vercode, vercode->attach_field(),
  REGISTER_VERCODE_RLY,
             george_logic::USER_TYPE);
  //发送短信
  SendSMSVercode(vercode->phone(), "注册码:",code);
  if (t_vercode) {
    delete t_vercode;
    t_vercode = NULL;
  }
  if (vercode) {
    delete vercode;
    vercode = NULL;
  }
}

void UserFactory::OnLoginVercode(const int socket,
                                 base_logic::DictionaryValue* dict,
                                 george_logic::PacketHead* packet) {
  user_logic::net_request::UserGetVerCode* vercode =
      new user_logic::net_request::UserGetVerCode();
  int32 code = 0;
  vercode->set_http_packet(dict);
  bool r = login_mgr_->CreateVerCode(vercode->phone(), code);
  //发送
  user_logic::net_reply::LoginVercode* t_vercode =
      new user_logic::net_reply::LoginVercode();
  t_vercode->set_vercode(code);
  if (r)
    t_vercode->set_status(1);
  else
    t_vercode->set_status(0);
  SendPacket(socket, t_vercode, vercode->attach_field(),
  LOGIN_VERCODE_RLY,
             george_logic::USER_TYPE);
  //发送短信
  SendSMSVercode(vercode->phone(), "登录码:",code);
  if (t_vercode) {
    delete t_vercode;
    t_vercode = NULL;
  }
  if (vercode) {
    delete vercode;
    vercode = NULL;
  }
}

void UserFactory::OnRegisterEvent(const int socket,
                                  base_logic::DictionaryValue* dict,
                                  george_logic::PacketHead* packet) {
  user_logic::net_request::UserRegister* user_register =
      new user_logic::net_request::UserRegister();
  int32 status = 0;
  user_register->set_http_packet(dict);
  bool r = register_mgr_->RegisterUser(user_register->phone(),
                                       user_register->password(),
                                       user_register->vercode());
  status = r ? 0 : -1;
  //发送
  user_logic::net_reply::RegisterUser* t_vercode =
      new user_logic::net_reply::RegisterUser();
  t_vercode->set_status(status);
  SendPacket(socket, t_vercode, packet->attach_field(),
  REGISTER_USER_RLY,
             george_logic::USER_TYPE);
  if (t_vercode) {
    delete t_vercode;
    t_vercode = NULL;
  }
  if (user_register) {
    delete user_register;
    user_register = NULL;
  }
}

void UserFactory::OnSMSLoginEvent(const int socket,
                                  base_logic::DictionaryValue* dict,
                                  george_logic::PacketHead* packet) {
  user_logic::net_request::SMSLogin* user_login =
      new user_logic::net_request::SMSLogin();
  int32 status = 0;
  user_logic::UserInfo info;
  user_login->set_http_packet(dict);
  bool r = login_mgr_->LoginSMSUser(user_login->phone(), user_login->vercode(),
                                    info);
  info.set_token("i1YXS90YALcR1IFd12618cf1bd782825f3550a81fc47a02");
  user_logic::net_reply::UserLogin* t_user_login =
      new user_logic::net_reply::UserLogin();
  if (r) {
    t_user_login->set_status(1);
    t_user_login->set_info(info);
  } else
    t_user_login->set_status(0);

  SendPacket(socket, t_user_login, packet->attach_field(),
  SMS_LOGIN_RLY,
             george_logic::USER_TYPE);
  if (t_user_login) {
    delete t_user_login;
    t_user_login = NULL;
  }
  if (user_login) {
    delete user_login;
    user_login = NULL;
  }
}

void UserFactory::OnLoginEvent(const int socket,
                               base_logic::DictionaryValue* dict,
                               george_logic::PacketHead* packet) {
  user_logic::net_request::UserLogin* user_login =
      new user_logic::net_request::UserLogin();
  int32 status = 0;
  user_logic::UserInfo info;
  user_login->set_http_packet(dict);
  bool r = login_mgr_->LoginUser(user_login->phone(), user_login->password(),
                                 info);
  info.set_token("i1YXS90YALcR1IFd12618cf1bd782825f3550a81fc47a02");
  user_logic::net_reply::UserLogin* t_user_login =
      new user_logic::net_reply::UserLogin();
  if (r) {
    t_user_login->set_status(1);
    t_user_login->set_info(info);
  } else
    t_user_login->set_status(0);

  SendPacket(socket, t_user_login, packet->attach_field(),
  LOGIN_USER_RLY,
             george_logic::USER_TYPE);
  if (t_user_login) {
    delete t_user_login;
    t_user_login = NULL;
  }
  if (user_login) {
    delete user_login;
    user_login = NULL;
  }
}

void UserFactory::SendPacket(const int socket, george_logic::PacketHead* packet,
                             george_logic::AttachField* attach,
                             const int16 operator_code, const int8 type) {
  packet->set_operator_code(operator_code);
  packet->set_type(type);
  if (attach != NULL && attach->format() == "jsonp") {
    packet->attach_field()->set_callback(attach->callback());
    packet_jsonp_->PackPacket(socket, packet->packet());
  } else
    packet_json_->PackPacket(socket, packet->packet());
}

void UserFactory::SendError(const int socket, george_logic::AttachField* attach,
                            const int16 operator_code) {
  SendHeader(socket, attach, operator_code, george_logic::ERROR_TYPE);
}

void UserFactory::SendHeader(const int socket,
                             george_logic::AttachField* attach,
                             const int16 operator_code, const int8 type) {
  george_logic::PacketHead *header = new george_logic::PacketHead();
  SendPacket(socket, header, attach, operator_code, type);
  if (header) {
    delete header;
    header = NULL;
  }
}

void UserFactory::SendSMSVercode(const std::string& phone,
                                 const std::string& type, const int32 vercode) {
  std::string result;
  std::string sign = "【追股神器】";
  std::string msg = sign + type +base::BasicUtil::StringUtil::Int64ToString(vercode)
      + " 有效期为10分钟，请在页面中输入以完成验证。验证码请勿泄露。如非本人操作请忽略";
  std::string url = "http://114.215.84.48/msg/HttpSendSM";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue;
  dict->SetString(L"account", "HYYunDian");
  dict->SetString(L"msg", msg);
  dict->SetString(L"pswd", "yun@dian@1");
  dict->SetString(L"mobile", phone);
  dict->SetString(L"needstatus", "false");
  base_http::HttpAPI::RequestPostMethod(url, dict, result);
}

}

