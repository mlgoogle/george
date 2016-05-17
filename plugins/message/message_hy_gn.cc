/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/runner/jindowin/jindowin/plugins/search/hand_search.cc
#    Author       : Sunsolo
#    Email        : 657974959@qq.com  我好饿！！
#    Date         : 2016-03-28 15:29
#    Description  : 
=============================================================================*/
#include "message/message_hy_gn.h"
#include "message/hy_gn_unit.h"
#include "db/db_comm.h"
#include "db/redis_comm.h"
#include "logic/logic_unit.h"
#include "net/message_comm.h"
#include "global/global.h"
#include "tools/tools.h"
#include "logic/logic_comm.h"

bool MessageHyGn::GetData() {
  bool r = DataBetweenRedisAndMySql(0, 0, 0);
  return r;
}

bool MessageHyGn::DataToHyGnUnit(HyGnUnit *hy_gn_unit, std::string &code) {
  hy_gn_unit->code_ = code;
  hy_gn_unit->name_ = global::GetInfo::GetInstance()->GetNameByCode(code);

  db::DbRedis redis;
  // visit
  int64_t visit_cur_num = redis.GetVsaNum(REDIS_VISIT, cur_time_, code, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    visit_cur_num = 0;
  }

  int64_t visit_pre_num = redis.GetVsaNum(REDIS_VISIT, pre_time_, code, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    visit_pre_num;
  }

  int64_t visit_total = redis.GetVsaNum(REDIS_VISIT, cur_time_, "", six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    visit_total = 0;
  }

  if (0 == visit_total)
    visit_total = 1;
  if (0 == visit_pre_num)
    visit_pre_num = 1;

  hy_gn_unit->vp_ = (visit_cur_num * 1.0) / visit_total;
  hy_gn_unit->vip_ = ((visit_cur_num - visit_pre_num) * 1.0) / visit_pre_num;

  //  search
  int64_t search_cur_num = redis.GetVsaNum(REDIS_SEARCH, cur_time_, code, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    search_cur_num;
  }

  int64_t search_pre_num = redis.GetVsaNum(REDIS_SEARCH, pre_time_, code, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    search_pre_num = 0;
  }

  int64_t search_total = redis.GetVsaNum(REDIS_SEARCH, cur_time_, "", six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    search_total = 0;
  }

  if (0 == search_total)
    search_total = 1;
  if (0 == search_pre_num)
    search_pre_num = 1;

  hy_gn_unit->sp_ = (search_cur_num * 1.0) / search_total;
  hy_gn_unit->sip_ = ((search_cur_num - search_pre_num) * 1.0) / search_pre_num;

  //  follow
  int64_t follow_cur_num = redis.GetVsaNum(REDIS_FOLLOW, cur_time_, code, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    follow_cur_num = 0;
  }

  int64_t follow_pre_num = redis.GetVsaNum(REDIS_FOLLOW, pre_time_, code, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    follow_pre_num = 0;
  }

  int64_t follow_total = redis.GetVsaNum(REDIS_FOLLOW, cur_time_, "", six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    follow_total = 0;
  }

  if (0 == follow_total)
    follow_total = 1;
  if (0 == follow_pre_num)
    follow_pre_num = 1;

  hy_gn_unit->fp_ = (follow_cur_num * 1.0) / follow_total;
  hy_gn_unit->fip_ = ((follow_cur_num - follow_pre_num) * 1.0) / follow_pre_num;
  LOG_DEBUG2("%ld, %ld, %ld", follow_cur_num, follow_pre_num, follow_total);

  error_info_ = 0;
}

bool MessageHyGn::DataBetweenRedisAndMySql(time_t start_time, \
                                               time_t end_time, \
                                            time_t middle_time) {
  db::DbSql sql;
  std::string code = sql.GetHyGnCode(hy_gn_, operate_);
  ContainerStr codes;
  tools::SeparatorStr<ContainerStr>(code, ',', &codes, false);

  HyGnUnit min_heap[OUT_MAX];
  int heap_count = 0;

  int len = codes.size();
  if (len > 0) {
    HyGnUnit temp;
    for(int i = 0; i < len; ++i) {
      code = codes[i];
      DataToHyGnUnit(&temp, code);
      tools::HandleHeap<HyGnUnit, HyGnUnitCompare>(min_heap, &heap_count, temp, OUT_MAX);
    }

    for(int i = 0; i < OUT_MAX && i < heap_count; ++i) {
      hy_gn_units_.push_back(min_heap[i]);
    }

    std::sort(hy_gn_units_.begin(), hy_gn_units_.end(), HyGnUnitCompare());
  }

  SubscribeNum_ = sql.GetHyGnSubscribeNum(hy_gn_, operate_);
  error_info_ = sql.get_error_info();
  if (0 != error_info_) return false;

  IfSubscribe_ = sql.HyGnIfSubscribe(user_id_, hy_gn_, operate_);
  error_info_ = sql.get_error_info();
  if (0 != error_info_) return false;
}

void MessageHyGn::SendMessage() {
  netcomm_send::SendMessageHyGn return_message;

  int len = hy_gn_units_.size();
  for (int i = 0; i < len; ++i) {
    return_message.set_restult(hy_gn_units_[i].Release());
  }

  return_message.set_SubscribeNum(SubscribeNum_);
  return_message.set_IfSubscribe(IfSubscribe_);
  
  int json_type = jsonp_str_ == "" ? 0 : 3;
  if (json_type == 3)
    return_message.set_jsonp_callback(jsonp_str_);

  send_message(socket_, &return_message, json_type);
}

