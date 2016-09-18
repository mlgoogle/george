//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#include "user_login_mgr.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include "basic/radom_in.h"
#include "thread/base_thread_lock.h"
#include <algorithm>

namespace user_logic {

LoginManager* LoginEngine::login_mgr_ = NULL;
LoginEngine* LoginEngine::login_engine_ = NULL;

LoginManager::LoginManager()
    : user_db_(NULL) {
  login_cache_ = new LoginCache();
  Init();
}
LoginManager::~LoginManager() {
  DeinitThreadrw(lock_);
}

void LoginManager::Init(user_logic::UserDB* user_db) {
  user_db_ = user_db;
}

void LoginManager::Init() {

  InitThreadrw(&lock_);
}

bool LoginManager::LoginUser(const std::string& phone,
                             const std::string& password,
                             user_logic::UserInfo& info) {

  return user_db_->LoginUser(phone, password, info);
}

bool LoginManager::LoginSMSUser(const std::string& phone, const int32 vercode,
                                                    user_logic::UserInfo& info) {
  bool r = false;
  user_logic::RegiserVercode rvercode;
  {
    base_logic::RLockGd lk(lock_);
    r = base::MapGet<VERCODE_MAP, VERCODE_MAP::iterator, std::string,
        user_logic::LoginVercode>(login_cache_->vercode_map_, phone, rvercode);
  }
  if (r) {  //检测是否超时
    if (!rvercode.cmp_dead_line())
      return false;
  }

  if(vercode != rvercode.vercode())
    return false;
  //读取用户信息

  return user_db_->LoginSMS(phone,info);
}

bool LoginManager::CreateVerCode(const std::string& phone, int32& vercode) {

  bool r = false;

  user_logic::RegiserVercode rvercode;
  {
    base_logic::RLockGd lk(lock_);
    r = base::MapGet<VERCODE_MAP, VERCODE_MAP::iterator, std::string,
        user_logic::LoginVercode>(login_cache_->vercode_map_, phone, rvercode);
  }
  if (r) {  //检测是否超时
    if (!rvercode.cmp_dead_line())
      return false;
  }

  //重新创建验证码
  int32 result_code = CreateVercode();
  rvercode.set_phone(phone);
  rvercode.set_vercode(result_code);
  rvercode.set_dead_line(time(NULL));
  {
    base_logic::WLockGd lk(lock_);
    r = base::MapAdd<VERCODE_MAP, std::string, user_logic::LoginVercode>(
        login_cache_->vercode_map_, phone, rvercode);
  }
  vercode = result_code;
  return true;
}

int32 LoginManager::CreateVercode() {
  int32 radom_num = base::SysRadom::GetInstance()->GetRandomIntID();
  return (radom_num % 10000);
}

}

