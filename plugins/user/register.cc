//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/3/28 Author: chenxun

#include "user/register.h"

#include <string>
#include <sstream>

#include "user/user_logic.h"
#include "logic/logic_unit.h"
#include "db/db_comm.h"
#include "tools/tools.h"
#include "net/typedef.h"
#include "net/comm_struct.h"
#include "net/user_comm_head.h"
#include "tea/tea.h"
#include "user/kafka_accessor.h"
#include "user/user_manager.h"

namespace user_register {
int Register::CheckUser() {
  do {
    db::DbSql sql;
    std::string username = "";
    real_passwd_ = "";
    username = sql.GetUserNameInfo(user_name_);
    error_info_ = sql.get_error_info();
    if (0 != error_info_) {
      break;
    }
    LOG_DEBUG2("--------GetUserNameInfo--------------user_name:%s",username.c_str());
    if(username == user_name_) {
      error_info_ = USER_NAME_EXIST;
      break;
    }
    for (int i = 0; i < password_.length(); ++i) {
      if (i == 2 || i == 15 || i == 32)
	continue;
      real_passwd_.push_back(password_[i]);
    }
    if (NULL == UserManager::GetInstance()->FindRegist(user_name_)) {
	UserInfo* user = new UserInfo;
	user->id = -1;
	user->email = user_name_;
	user->passwd = real_passwd_;
	user->level = 0;
	UserManager::GetInstance()->AddRegist(user);
    }
    // send verify email
    error_info_ = 0;
    SendActivrEmail();
//  sql.SetUserIfo(platform_id_, user_name_,password);
  } while(0);
  return error_info_;
}

int Register::InsertUser() {
  for (int i = 0; i < password_.length(); ++i) {
     if (i == 2 || i == 15 || i == 32)
       continue;
     real_passwd_.push_back(password_[i]);
   }
  db::DbSql sql;
  sql.SetUserIfo(platform_id_, user_name_,real_passwd_);
  return 0;
}

void Register::SendMessage() {
  SendPacketBase send;
  send.set_msg("");
  send.set_status(1);
  send_message(socket_, &send);
}

void Register::SendActivrEmail() {
  std::stringstream ss;
//  std::string content = "";
//  std::string email_html = user_svc_logic::Userlogic::GetInstance()->email_html_;
//  char* html = new char[email_html.length() + 512];
//  memset(html, 0, email_html.length() + 512);
//  sprintf(html, email_html.c_str(), user_name_, "www.baidu.com");
//  content = html;
//  delete[] html;

  ss << "{" \
     << "\"subject\":\"" << "激活邮箱" << "\"," \
     << "\"account\":\"" << user_name_ <<"\"," \
     << "\"verifyurl\":\"" << CreateActiveUrl() <<"\"," \
     << "\"to\":\"" << user_name_ << "\"" \
     << "}";
  LOG_DEBUG2("push kafka data:%s",ss.str().c_str());
  int n = user::KafkaAccessor::PushData(ss.str().c_str(), ss.str().length());
}

//
std::string Register::CreateActiveUrl() {
  std::string url = active_url_;
  url.append("?email=");
  url.append(user_name_);
  url.append("&param=");

  int64 time = tools::CurrentTime();
  std::string param;
  std::stringstream ss;

  ss << user_name_<< ";"
      << password_ << ";"
      << platform_id_ << ";"
      << time;
  url.append(tools::TeaEncode(ss.str()));
  return url;
}

}

