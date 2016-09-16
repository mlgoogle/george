// Copyright (c) 2016 The george Authors. All rights reserved.
// news_proto_buf.cc
// Created on: 2016年5月30日.
// Author: Paco.

#include "news/news_proto_buf.h"
#include "basic/basic_util.h"
#include "logic/logic_comm.h"

namespace news {

namespace recv {

void RelateNews::set_http_packet(DicValue* dic) {
  dic->GetBigInteger(L"uid",&uid_);
  dic->GetString(L"token",&token_);
  int64 n;
  dic->GetBigInteger(L"count", &n);
  count_ = n;
  dic->GetBigInteger(L"last_id", &last_id_);
  int64 m;
  dic->GetBigInteger(L"relate", &m);
  relate_ = m;
  std::string stmp;
  bool r = dic->GetString(L"key_name",&stmp);
  if (r)
    base::BasicUtil::UrlDecode(stmp, key_name_);
  key_name_ = key_name_.substr(0, key_name_.length() - 1);
  LOG_DEBUG2("count[%d]last_id[%d]relate[%d]key_name[%s]", count_, last_id_,
             relate_, key_name_.c_str());
}

void StNews::set_http_packet(DicValue* dic) {
  dic->GetBigInteger(L"uid",&uid_);
  dic->GetString(L"token",&token_);
  std::string stmp;
  bool r = dic->GetString(L"stock",&stmp);
  if (r)
    base::BasicUtil::UrlDecode(stmp, stock_);
  stock_ = stock_.substr(0, stock_.length() - 1);
}

}  // namespace recv

}  // namespace news

