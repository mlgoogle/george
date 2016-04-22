/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/runner/jindowin/jindowin/plugins/search/handle_search.h
#    Author       : Sunsolo
#    Email        : wukun@kunyan-inc.com  我好饿！！
#    Date         : 2016-03-28 15:18
#    Description  : 
=============================================================================*/

#ifndef _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_HANDLE_CODE_H_
#define _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_HANDLE_CODE_H_

#include <stdint.h>

#include <string>

#include "net/base.h"

class MessageCode : public base::Base {
  public:
    MessageCode(int64 user_id, \
                 std::string code, \
             std::string jsonp_str, \
                          int socket) 
      : user_id_(user_id), \
              code_(code), \
    jsonp_str_(jsonp_str), \
        socket_(socket) {
          six_type_ = 0;
        }

    ~MessageCode() {}
  public:
    bool GetData();
    void SendMessage();
  private:
    bool DataInMySql(time_t start_time, time_t end_time) {}
    bool DataInRedis(time_t start_time, time_t end_time) {}
    bool DataBetweenRedisAndMySql(time_t start_time, \
                                    time_t end_time, \
                                  time_t middle_time);

  private:
    int64 user_id_;

    int64 SubscribeNum_;
    int64 IfSubscribe_;

    double vp_;
    double vip_;

    double sp_;
    double sip_;

    double fp_;
    double fip_;

    std::string code_;
    std::string jsonp_str_;
    int socket_;

    int six_type_;

};

#endif  //  _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_HANDLE_CODE_H_
