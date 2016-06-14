// Copyright (c) 2016 The george Authors. All rights reserved.
// news_proto_buf.cc
// Created on: 2016年5月30日.
// Author: Paco.

#include "search/search_proto_buf.h"
#include "public/basic/basic_util.h"
#include "base/logic/logic_comm.h"

namespace search {

namespace recv {

void BaseRecv::set_http_packet(DicValue* dic) {
  dic->GetBigInteger(L"uid",&uid_);
  dic->GetString(L"token",&token_);
}

void SearchKey::set_http_packet(DicValue* dic) {
  dic->GetBigInteger(L"uid",&uid_);
  dic->GetString(L"token",&token_);
  std::string stmp;
  bool r = dic->GetString(L"key_name",&stmp);
  if (r)
    base::BasicUtil::UrlDecode(stmp, key_name_);
  key_name_ = key_name_.substr(0, key_name_.length() - 1);
  LOG_DEBUG2("key_name[%s]", key_name_.c_str());
}

void AlterSubscribe::set_http_packet(DicValue* dic) {
  dic->GetBigInteger(L"uid",&uid_);
  dic->GetString(L"token",&token_);
  dic->GetBigInteger(L"alter_type", &alter_type_);
  std::string stmp;
  dic->GetString(L"code", &stock_);
  if (stock_.length() > 0)
    stock_ = stock_.substr(0, stock_.length() - 1);

}

}  // namespace recv

}  // namespace news

