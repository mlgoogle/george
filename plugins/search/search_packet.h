// Copyright (c) 2016 The george Authors. All rights reserved.
// search_packet.h
// Created on: 2016年6月6日.
// Author: Paco.
#ifndef PLUGINS_SEARCH_SEARCH_PACKET_H_
#define PLUGINS_SEARCH_SEARCH_PACKET_H_

#include "pub/net/typedef.h"
#include "pub/net/proto_buf.h"

namespace search {

class SearchPacket {
 public:
  SearchPacket();
  ~SearchPacket();
 public:
  static void PacketPocessGet(const int socket, DicValue* dict,
     george_logic::PacketHead* packet);
};

}  // namespace search



#endif  // PLUGINS_SEARCH_SEARCH_PACKET_H_
