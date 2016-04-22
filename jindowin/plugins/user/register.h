//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2016/3/28 Author: chenxun

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_REGISTER_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_REGISTER_H_

#include <string>

#include "net/base.h"
#include "basic/basictypes.h"

namespace user_register {
class Register {
 public:
  Register() \
    : platform_id_(-1), \
    user_id_(-1), \
    user_level_(-1), \
    socket_(-1) {}
  Register(int platform_id,
	   const std::string &user_name,
	   const std::string &password,
	   const std::string &jsonp_str, int socket) \
    : platform_id_(platform_id), \
    user_name_(user_name), \
    password_(password), \
    user_id_(-1), \
    user_level_(-1), \
    jsonp_str_(jsonp_str), \
    socket_(socket) {}
  virtual ~Register() {}
  //  锟斤拷锟斤拷涌锟�
 public:
  int CheckUser();
  int InsertUser();
  void SendMessage();
  void SendActivrEmail();

 private:
  std::string CreateActiveUrl();

public:
  int platform_id_;
  std::string user_name_;
  std::string password_;
  int64 user_id_;
  int user_level_;
  int socket_;
  std::string jsonp_str_;
  std::string real_passwd_;
  int32 error_info_;
};

}  // namespace login

#endif
