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
#include "message/message_code.h"
#include "db/db_comm.h"
#include "db/redis_comm.h"
#include "logic/logic_unit.h"
#include "net/typedef.h"
#include "net/message_comm.h"
#include "global/global.h"
#include "tools/tools.h"
#include "logic/logic_comm.h"

bool MessageCode::GetData() {
  bool r = DataBetweenRedisAndMySql(0, 0, 0);
  return r;
}

bool MessageCode::DataBetweenRedisAndMySql(time_t start_time, \
                                             time_t end_time, \
                                          time_t middle_time) {
  db::DbSql sql;
  SubscribeNum_ = sql.GetCodeSubscribeNum(code_);
  error_info_ = sql.get_error_info();
  if (0 != error_info_) return false;

  IfSubscribe_ = sql.CodeIfSubscribe(user_id_, code_);
  error_info_ = sql.get_error_info();
  if (0 != error_info_) return false;

  time_t cur_time = time(NULL);
  cur_time = cur_time - (cur_time % 3600) - REDIS_DELAY;
  time_t pre_time = cur_time - 3600;

  db::DbRedis redis;
  // visit
  int64_t visit_cur_num = redis.GetVsaNum(REDIS_VISIT, cur_time, code_, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    visit_cur_num = 0;
  }

  int64_t visit_pre_num = redis.GetVsaNum(REDIS_VISIT, pre_time, code_, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    visit_pre_num = 0;
  }

  int64_t visit_total = redis.GetVsaNum(REDIS_VISIT, cur_time, "", six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    visit_total = 0;
  }

  if (0 == visit_total)
    visit_total = 1;
  if (0 == visit_pre_num)
    visit_pre_num = 1;

  vp_ = (visit_cur_num * 1.0) / visit_total;
  vip_ = ((visit_cur_num - visit_pre_num) * 1.0) / visit_pre_num;

  //  search
  int64_t search_cur_num = redis.GetVsaNum(REDIS_SEARCH, cur_time, code_, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    search_cur_num = 0;
  }

  int64_t search_pre_num = redis.GetVsaNum(REDIS_SEARCH, pre_time, code_, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    search_pre_num = 0;
  }

  int64_t search_total = redis.GetVsaNum(REDIS_SEARCH, cur_time, "", six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    search_total = 0;
  }

  if (0 == search_total)
    search_total = 1;
  if (0 == search_pre_num)
    search_pre_num = 1;

  sp_ = (search_cur_num * 1.0) / search_total;
  sip_ = ((search_cur_num - search_pre_num) * 1.0) / search_pre_num;

  //  follow
  int64_t follow_cur_num = redis.GetVsaNum(REDIS_FOLLOW, cur_time, code_, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    follow_cur_num = 0;
  }

  int64_t follow_pre_num = redis.GetVsaNum(REDIS_FOLLOW, pre_time, code_, six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    follow_pre_num = 0;
  }

  int64_t follow_total = redis.GetVsaNum(REDIS_FOLLOW, cur_time, "", six_type_);
  error_info_ = redis.get_error_info();
  if (0 != error_info_) {
    follow_total = 0;
  }

  if (0 == follow_total)
    follow_total = 1;
  if (0 == follow_pre_num)
    follow_pre_num = 1;

  fp_ = (follow_cur_num * 1.0) / follow_total;
  fip_ = ((follow_cur_num - follow_pre_num) * 1.0) / follow_pre_num;

  error_info_ = 0;
}

void MessageCode::SendMessage() {
  netcomm_send::SendMessageCode return_message;

  return_message.set_SubscribeNum(SubscribeNum_);
  return_message.set_IfSubscribe(IfSubscribe_);

  return_message.set_vp(vp_);
  return_message.set_vip(vip_);

  return_message.set_sp(sp_);
  return_message.set_sip(sip_);

  return_message.set_fp(fp_);
  return_message.set_fip(fip_);
  
  int json_type = jsonp_str_ == "" ? 0 : 3;
  if (json_type == 3)
    return_message.set_jsonp_callback(jsonp_str_);

  send_message(socket_, &return_message, json_type);
}

