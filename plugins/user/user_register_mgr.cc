//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#include "user_register_mgr.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include "basic/radom_in.h"
#include "thread/base_thread_lock.h"
#include <algorithm>

namespace user_logic {

RegisterManager* RegisterEngine::register_mgr_ = NULL;
RegisterEngine* RegisterEngine::register_engine_ = NULL;

RegisterManager::RegisterManager()
    : user_db_(NULL) {
  register_cache_ = new RegisterCache();
  Init();
}
RegisterManager::~RegisterManager() {
  DeinitThreadrw(lock_);
}

void RegisterManager::Init(user_logic::UserDB* user_db) {
  user_db_ = user_db;
}

void RegisterManager::Init() {

  InitThreadrw(&lock_);
}

bool RegisterManager::RegisterUser(const std::string& phone,
                                    const std::string& password,
                                    const int32 vercode) {
  bool r = false;
  user_logic::RegiserVercode rvercode;
  {
    base_logic::RLockGd lk(lock_);
    r = base::MapGet<VERCODE_MAP, VERCODE_MAP::iterator, std::string,
        user_logic::RegiserVercode>(register_cache_->vercode_map_, phone,
                                    rvercode);
  }
  if(vercode != rvercode.vercode())
    return false;
  //写入数据库
  user_db_->RegisterUser(phone,password);
  return true;
}

bool RegisterManager::CreateVerCode(const std::string& phone, int32& vercode) {

  bool r = false;

  user_logic::RegiserVercode rvercode;
  {
    base_logic::RLockGd lk(lock_);
    r = base::MapGet<VERCODE_MAP, VERCODE_MAP::iterator, std::string,
        user_logic::RegiserVercode>(register_cache_->vercode_map_, phone,
                                    rvercode);
  }
  if (r) {  //检测是否超时
    if (!rvercode.cmp_dead_line())
      return false;
  }

  //重新创建验证码
  int32 result_code = CreateVercode();
  rvercode.set_phone(phone);
  rvercode.set_vercode(result_code);
  {
    base_logic::WLockGd lk(lock_);
    r = base::MapAdd<VERCODE_MAP, std::string, user_logic::RegiserVercode>(
        register_cache_->vercode_map_, phone, rvercode);
  }
  vercode = result_code;
  return true;
}

int32 RegisterManager::CreateVercode() {
  int32 radom_num = base::SysRadom::GetInstance()->GetRandomIntID();
  return (radom_num % 10000);
}

}

