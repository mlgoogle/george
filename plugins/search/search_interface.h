// Copyright (c) 2016 The george Authors. All rights reserved.
// new_interface.h
// Created on: 2016年5月25日.
// Author: Paco.
#ifndef PLUGINS_SEARCH_SEARCH_INTERFACE_H_
#define PLUGINS_SEARCH_SEARCH_INTERFACE_H_

#include "config/config.h"

#include "search/search_mysql.h"
#include "search/search_memery.h"

#include "net/packet_process.h"

namespace search {

class SearchInterface {
 public:
  static SearchInterface* GetInstance();
  static void FreeInstance();

  void InitConfig(config::FileConfig* config);
  int InitMem();
  void Init();

  void OnAlterSubscribeEvent(const int socket, DicValue* dic,
                             george_logic::PacketHead* packet);

  void OnHotSubscribeEvent(const int socket, DicValue* dic,
                            george_logic::PacketHead* packet);

  void OnSubscribeEvent(const int socket, DicValue* dic,
                     george_logic::PacketHead* packet);
  void OnSearchEvent(const int socket, DicValue* dic,
                         george_logic::PacketHead* packet);

  void UpdateStockPrice();
  void Test();
 private:
  SearchInterface();
  ~SearchInterface();

  void SendPacket(const int socket, george_logic::PacketHead* packet,
      george_logic::AttachField* attach,
      const int16 operator_code,const int8 type);
 public:
  void SendError(const int socket,george_logic::AttachField* attach,
      const int16 operator_code);

  void SendHeader(const int socket,george_logic::AttachField* attach,
      const int16 operator_code, const int8 type);

  SearchMysql* search_mysql_;
  SearchMemery* search_memery_;
  george_logic::json_packet::PacketProcess* packet_json_;
  george_logic::jsonp_packet::PacketProcess* packet_jsonp_;

  static SearchInterface* instance_;
};

}  // namespace search



#endif  // PLUGINS_SEARCH_SEARCH_INTERFACE_H_
