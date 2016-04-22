
#ifndef _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_HY_HANDLE_HY_H_
#define _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_HY_HANDLE_HY_H_

#include <stdint.h>
#include <math.h>

#include <string>

#include "net/base.h"
#include "net/typedef.h"
#include "message/hy_gn_unit.h"

#define OUT_MAX 3
#define EPSILON 0.0000001

struct HyGnUnitCompare {  //  仿函数用于stl堆的比较
  bool operator()(const class HyGnUnit &a, \
                  const class HyGnUnit &b) {
    if (!(fabs(a.fp_ - b.fp_) < EPSILON)) {
      return a.fp_ > b.fp_;
    }

    if (!(fabs(a.sp_ - b.sp_) < EPSILON)) {
      return a.sp_ > b.sp_;
    }

    if (!(fabs(a.vp_ - b.vp_) < EPSILON)) {
      return a.vp_ > b.vp_;
    }

    return true;
  }
};

class MessageHyGn : public base::Base {
  public:
    MessageHyGn(int64 user_id, std::string hy_gn, \
                int64 operate, \
              std::string jsonp_str, int socket) 
              : user_id_(user_id), hy_gn_(hy_gn), operate_(operate), \
              jsonp_str_(jsonp_str), socket_(socket) {
      six_type_ = 0;

      cur_time_ = time(NULL);
      cur_time_ = cur_time_ - (cur_time_ % 3600) - REDIS_DELAY;
      pre_time_ = cur_time_ - 3600;
    }

    ~MessageHyGn() {}
  public:
    bool GetData();
    void SendMessage();

  private:
    bool DataInMySql(time_t start_time, time_t end_time) {;}
    bool DataInRedis(time_t start_time, time_t end_time) {;}
    bool DataBetweenRedisAndMySql(time_t start_time, \
                                    time_t end_time, \
                                  time_t middle_time);

    bool DataToHyGnUnit(HyGnUnit *hy_gn_unit, std::string &code);

  private:
    int64 user_id_;

    ContainerHyGnUnit hy_gn_units_;
    int64 SubscribeNum_;
    int64 IfSubscribe_;

    std::string hy_gn_;
    int64 operate_;

    std::string jsonp_str_;
    int socket_;

    int six_type_;

    time_t cur_time_;
    time_t pre_time_;
};

#endif  //  _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_HANDLE_HY_H_
