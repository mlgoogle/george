// Copyright (c) 2016 The Jindowin Authors. All rights reserved.
// kafka_accessor.h
// Created on: 2016骞�4鏈�19鏃�.
// Author: Paco.
#ifndef PLUGINS_USERLOGIN_KAFKA_ACCESSOR_H_
#define PLUGINS_USERLOGIN_KAFKA_ACCESSOR_H_

#include <list>
#include <string>

#include "base/kafka/kafka_producer.h"
namespace user {
class KafkaAccessor {
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

} // namespace user





#endif /* PLUGINS_USERLOGIN_KAFKA_ACCESSOR_H_ */
