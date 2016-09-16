// Copyright (c) 2016 The george Authors. All rights reserved.
// news_packet.cc
// Created on: 2016年5月28日.
// Author: Paco.

#include "news/news_packet.h"
#include "news/operator_code.h"
#include "news/news_interface.h"

namespace news {

NewsPacket::NewsPacket() {
}

NewsPacket::~NewsPacket() {

}

void NewsPacket::PacketPocessGet(const int socket, DicValue* dict,
      george_logic::PacketHead* packet) {
  int16 operate_code = packet->operate_code();
  NewsInterface* interface = NewsInterface::GetInstance();

  switch (operate_code) {
   case RELATE_NEWS_REQ: {
     interface->OnRelateNewsEvent(socket, dict, packet);
     break;
   }
   case RELATE_COUNT_REQ: {
     interface->OnStNewsEvent(socket, dict, packet);
     break;
   }
   case NEWS_DETAILS_REQ: {
     interface->OnNewsDetailsEvent(socket, dict, packet);
     break;
   }
   default:
    break;
  }
}

}  // namespace news
