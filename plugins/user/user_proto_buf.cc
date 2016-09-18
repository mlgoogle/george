//  Copyright (c) 2015-2015 The GEORGE Authors. All rights reserved.
//  Created on: 2016年5月25日 Author: kerry

#include "user_proto_buf.h"
#include "basic/basic_util.h"

namespace user_logic {

namespace net_request {

void UserGetVerCode::set_http_packet(base_logic::DictionaryValue* value){
  bool r = false;
  int64 phone;

  r = value->GetBigInteger(L"phone", &phone);
  if(r)
    set_phone(base::BasicUtil::StringUtil::Int64ToString(phone));
}

void UserRegister::set_http_packet(base_logic::DictionaryValue* value){
  bool r = false;
  int64 phone;
  std::string password;
  int64 vercode;
  r = value->GetBigInteger(L"phone", &phone);
    if(r)
      set_phone(base::BasicUtil::StringUtil::Int64ToString(phone));

  r = value->GetString(L"password", &password);
  if(r)
    set_password(password);
  r = value->GetBigInteger(L"vercode", &vercode);
  if(r)
    set_vercode(vercode);
}

void UserLogin::set_http_packet(base_logic::DictionaryValue* value){
  bool r = false;
  int64 phone;
  std::string password;
  int64 vercode;
  r = value->GetBigInteger(L"phone", &phone);
    if(r)
      set_phone(base::BasicUtil::StringUtil::Int64ToString(phone));

  r = value->GetString(L"password", &password);
  if(r)
    set_password(password);
}

void SMSLogin::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
   int64 phone;
   int64 vercode;
   r = value->GetBigInteger(L"phone", &phone);
     if(r)
       set_phone(base::BasicUtil::StringUtil::Int64ToString(phone));

   r = value->GetBigInteger(L"vercode", &vercode);
   if(r)
     set_vercode(vercode);
}

}
}
