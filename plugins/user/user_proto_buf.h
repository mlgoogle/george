//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#ifndef GEORGE_USER_PROTO_BUF_H__
#define GEORGE_USER_PROTO_BUF_H__

#include "user_info.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/proto_buf.h"
//#include "user_basic_info.h"
#include <list>
#include <string>

namespace user_logic {

namespace net_request {

class UserGetVerCode : public george_logic::PacketHead {
 public:
  UserGetVerCode()
      : phone_(NULL) {
  }

  virtual ~UserGetVerCode() {
    if (phone_) {
      delete phone_;
      phone_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_phone(const std::string& phone) {
    phone_ = new base_logic::StringValue(phone);
  }

  const std::string phone() const {
    std::string phone;
    if (phone_ != NULL)
      phone_->GetAsString(&phone);
    return phone;
  }

 public:
  base_logic::StringValue* phone_;
};

class SMSLogin : public george_logic::PacketHead{
 public:
  SMSLogin()
      : phone_(NULL),
        vercode_(NULL) {
  }

  ~SMSLogin() {
    if (phone_) {
      delete phone_;
      phone_ = NULL;
    }
    if (vercode_) {
      delete vercode_;
      vercode_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_phone(const std::string& phone) {
    phone_ = new base_logic::StringValue(phone);
  }

  void set_vercode(const int32& vercode) {
    vercode_ = new base_logic::FundamentalValue(vercode);
  }


  const std::string phone() const {
    std::string phone;
    if (phone_ != NULL)
      phone_->GetAsString(&phone);
    return phone;
  }

  const int32 vercode() const {
    int32 vercode;
    if (vercode_ != NULL)
      vercode_->GetAsInteger(&vercode);
    return vercode;
  }
 public:
  base_logic::StringValue* phone_;
  base_logic::FundamentalValue* vercode_;
};


class UserLogin : public george_logic::PacketHead{
 public:
  UserLogin()
      : phone_(NULL),
        password_(NULL) {
  }

  ~UserLogin() {
    if (phone_) {
      delete phone_;
      phone_ = NULL;
    }
    if (password_) {
      delete password_;
      password_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_phone(const std::string& phone) {
    phone_ = new base_logic::StringValue(phone);
  }

  void set_password(const std::string& password) {
    password_ = new base_logic::StringValue(password);
  }


  const std::string phone() const {
    std::string phone;
    if (phone_ != NULL)
      phone_->GetAsString(&phone);
    return phone;
  }

  const std::string password() const {
    std::string password;
    if (password_ != NULL)
      password_->GetAsString(&password);
    return password;
  }
 public:
  base_logic::StringValue* phone_;
  base_logic::StringValue* password_;
};

class UserRegister : public george_logic::PacketHead {
 public:
  UserRegister()
      : phone_(NULL),
        password_(NULL),
        vercode_(NULL) {
  }

  ~UserRegister() {
    if (phone_) {
      delete phone_;
      phone_ = NULL;
    }
    if (password_) {
      delete password_;
      password_ = NULL;
    }
    if (vercode_) {
      delete vercode_;
      vercode_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_phone(const std::string& phone) {
    phone_ = new base_logic::StringValue(phone);
  }

  void set_password(const std::string& password) {
    password_ = new base_logic::StringValue(password);
  }

  void set_vercode(const int32 vercode) {
    vercode_ = new base_logic::FundamentalValue(vercode);
  }

  const std::string phone() const {
    std::string phone;
    if (phone_ != NULL)
      phone_->GetAsString(&phone);
    return phone;
  }

  const std::string password() const {
    std::string password;
    if (password_ != NULL)
      password_->GetAsString(&password);
    return password;
  }

  const int32 vercode() const {
    int32 ver_code = 0;
    if (vercode_ != NULL)
      vercode_->GetAsInteger(&ver_code);
    return ver_code;
  }

 public:
  base_logic::StringValue* phone_;
  base_logic::StringValue* password_;
  base_logic::FundamentalValue* vercode_;
};

}

namespace net_reply {
class UserLogin : public george_logic::PacketHead {
 public:
  UserLogin() {
    status_ = NULL;
    info_ = NULL;
  }

  ~UserLogin() {
    if (body_value_) {
      delete body_value_;
      body_value_ = NULL;
    }
  }

  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    if (status_ != NULL)
      body_value_->Set(L"status", status_);
    if (info_ != NULL)
      body_value_->Set(L"info", info_);
    return body_value_;
  }

  void set_info(user_logic::UserInfo& info) {
    info_ = new base_logic::DictionaryValue();
    info_->SetBigInteger(L"id",info.uid());
    info_->SetString(L"phone",info.phone());
    info_->SetString(L"token",info.token());
  }
  void set_status(int32 status) {
    status_ = new base_logic::FundamentalValue(status);
  }
 public:
  base_logic::DictionaryValue* info_;
  base_logic::FundamentalValue* status_;
};

class RegisterVercode : public george_logic::PacketHead {
 public:
  RegisterVercode() {
    vercode_ = NULL;
    status_ = NULL;
  }

  ~RegisterVercode() {
    if (body_value_) {
      delete body_value_;
      body_value_ = NULL;
    }
  }

  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    if (vercode_ != NULL)
      body_value_->Set(L"vercode", vercode_);
    if (status_ != NULL)
      body_value_->Set(L"status", status_);
    return body_value_;
  }

  void set_vercode(int32 vercode) {
    vercode_ = new base_logic::FundamentalValue(vercode);
  }
  void set_status(int32 status) {
    status_ = new base_logic::FundamentalValue(status);
  }
 public:
  base_logic::FundamentalValue* vercode_;
  base_logic::FundamentalValue* status_;
};

typedef RegisterVercode RegisterUser;
typedef RegisterVercode LoginVercode;
}

}
#endif
