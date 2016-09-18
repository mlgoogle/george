//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#include "user_info.h"

namespace user_logic {

RegiserVercode::RegiserVercode() {
  data_ = new Data();
}

RegiserVercode::RegiserVercode(const RegiserVercode& vercode)
    : data_(vercode.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

RegiserVercode& RegiserVercode::operator =(const RegiserVercode& vercode) {
  if (vercode.data_ != NULL) {
    vercode.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = vercode.data_;
  return (*this);
}

UserInfo::UserInfo() {
  data_ = new Data();
}

UserInfo::UserInfo(const UserInfo& info)
    : data_(info.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

UserInfo& UserInfo::operator =(const UserInfo& info) {
  if (info.data_ != NULL) {
    info.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = info.data_;
  return (*this);
}

void UserInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetBigInteger(L"uid", &data_->uid_);
  dict->GetString(L"phone", &data_->phone_);

}

}

