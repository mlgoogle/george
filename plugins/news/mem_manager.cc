//  Copyright (c) 2015-2015 The mem_manager.cc Authors. All rights reserved.
//  Created on: 2016年1月9日 Author: paco

#include "../news/mem_manager.h"

#include "../news/db_mysql.h"
#include "base/logic/logic_comm.h"

MemManager* MemManager::instance_;
pthread_mutex_t MemManager::lock_ = PTHREAD_MUTEX_INITIALIZER;
MemManager::MemManager() {
}

MemManager::~MemManager() {
}

MemManager* MemManager::GetInstance() {
  if (NULL == instance_) {
    pthread_mutex_lock(&lock_);
    if (NULL == instance_)
      instance_ = new MemManager();
    pthread_mutex_unlock(&lock_);
  }
  return instance_;
}

void MemManager::InitHyMap() {
  news::NewsMysql::QueryStockHy(&hy_stock_map_);
  LOG_DEBUG2("InitHyMap,size:%d", hy_stock_map_.size());
}

std::string MemManager::GetHyByCode(std::string code) {
  std::string name;
  StringMap::iterator it = hy_stock_map_.find(code);
  if (it != hy_stock_map_.end())
    name = it->second;
  return name;
}

std::string MemManager::GetGnByCode(std::string code) {
  std::string gnname;
  StringMap::iterator it = gn_stock_map_.find(code);
  if (it != gn_stock_map_.end())
    gnname = it->second;
  LOG_DEBUG2("GetGnByCode, code=%s, gn=%s", code.c_str(), gnname.c_str());
  return gnname;
}

void MemManager::InitGnMap() {
  news::NewsMysql::QueryStockGn(&gn_stock_map_);
  LOG_DEBUG2("InitGnMap,size:%d", gn_stock_map_.size());
}

void MemManager::InitStockNameMap() {
  news::NewsMysql::QueryStockName(&stock_name_map_);
  LOG_DEBUG2("InitStockNameMap,size:%d", stock_name_map_.size());
}

std::string MemManager::GetStockNameByCode(std::string code) {
  std::string name;
  StringMap::iterator it = stock_name_map_.find(code);
  if (it != stock_name_map_.end())
    name = it->second;
  return name;
}

std::string MemManager::GetUserSubscribe(int user_id, int get_type) {
  std::string result;
  return result;
}

