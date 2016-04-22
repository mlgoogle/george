/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/runner/northsea/restful/plugins/news/kafka_accessor.h
#    Author       : tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-03-12 09:39
#    Description  : 
=============================================================================*/

#ifndef _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_KAFKA_ACCESSOR_H_
#define _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_KAFKA_ACCESSOR_H_

#include <pthread.h>

#include <string>
#include <sstream>
#include <list>

#include "kafka_producer.h"

namespace news {

class KafkaAccessor{
 public:
  KafkaAccessor() {}
  ~KafkaAccessor() {}

 public:
  static int Init(int partition, std::string host, \
        int port, std::string topic);
  static int PushData(const char *content, const int content_length);
  static void Release();

 public:
  static std::list<kafka_producer*> s_producer_list_;
  static pthread_mutex_t s_kafka_mutext_;
  static int s_partition;
  static std::string s_broker;
  static std::string s_topic;
};

}  // namespace news
#endif  // _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_KAFKA_ACCESSOR_H_
