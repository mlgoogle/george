// Copyright (c) 2016 The george Authors. All rights reserved.
// search_packet.cc
// Created on: 2016年6月6日.
// Author: Paco.

#include "search/search_packet.h"
#include "search/operator_code.h"
#include "search/search_interface.h"

namespace search {

SearchPacket::SearchPacket() {
}

SearchPacket::~SearchPacket() {

}

void SearchPacket::PacketPocessGet(const int socket, DicValue* dict,
      george_logic::PacketHead* packet) {
  int16 operate_code = packet->operate_code();
  SearchInterface* interface = SearchInterface::GetInstance();

  switch (operate_code) {
   case SEARCH_REQ: {
     interface->OnSearchEvent(socket, dict, packet);
     break;
   }
   case QUERY_SUBSCRIBE_REQ: {
     interface->OnSubscribeEvent(socket, dict, packet);
     break;
   }
   case HOT_SUBSCRIBE_REQ: {
     interface->OnHotSubscribeEvent(socket, dict, packet);
     break;
   }
   case ALTER_SUBSCRIBE_REQ: {
     interface->OnAlterSubscribeEvent(socket, dict, packet);
     break;
   }
   default:
    break;
  }
}

}  // namespace search
