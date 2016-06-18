// Copyright (c) 2016 The george Authors. All rights reserved.
// new_interface.h
// Created on: 2016年5月25日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_INTERFACE_H_
#define PLUGINS_NEWS_CHECK_NEWS_INTERFACE_H_

#include "public/config/config.h"

#include "news_check/news_mysql.h"
#include "news_check/news_memery.h"

#include "pub/net/packet_process.h"

namespace news {

class NewsInterface {
 public:
  static NewsInterface* GetInstance();
  static void FreeInstance();

  void InitConfig(config::FileConfig* config);
  int InitNewsMem();
  void UpdateNews();
  void Init();

  void OnStNewsEvent(const int socket, DicValue* dic,
                     george_logic::PacketHead* packet);
  void OnRelateNewsEvent(const int socket, DicValue* dic,
                         george_logic::PacketHead* packet);
  void Test();
 private:
  NewsInterface();
  ~NewsInterface();

  void SendPacket(const int socket, george_logic::PacketHead* packet,
      george_logic::AttachField* attach,
      const int16 operator_code,const int8 type);
 public:
  void SendError(const int socket,george_logic::AttachField* attach,
      const int16 operator_code);

  void SendHeader(const int socket,george_logic::AttachField* attach,
      const int16 operator_code, const int8 type);

  NewsMysql* news_mysql_;
  NewsMemery* news_memery_;
  george_logic::json_packet::PacketProcess* packet_json_;
  george_logic::jsonp_packet::PacketProcess* packet_jsonp_;

  static NewsInterface* instance_;
};

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_NEWS_INTERFACE_H_
