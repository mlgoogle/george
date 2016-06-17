// Copyright (c) 2016 The george Authors. All rights reserved.
// news_packet.h
// Created on: 2016年5月28日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_PACKET_H_
#define PLUGINS_NEWS_CHECK_NEWS_PACKET_H_

#include "pub/net/typedef.h"
#include "pub/net/proto_buf.h"

namespace news {

class NewsPacket {
 public:
  NewsPacket();
  ~NewsPacket();
 public:
  static void PacketPocessGet(const int socket, DicValue* dict,
     george_logic::PacketHead* packet);
};

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_NEWS_PACKET_H_
