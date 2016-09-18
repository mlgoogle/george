//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#ifndef GEORGE_USER_INFO_H_
#define GEORGE_USER_INFO_H_

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include "thread/base_thread_lock.h"
#include <list>
#include <string>

namespace user_logic {

class RegiserVercode {
 public:
  RegiserVercode();
  RegiserVercode(const RegiserVercode& vercode);
  RegiserVercode& operator =(const RegiserVercode& vercode);

  ~RegiserVercode() {
    if (data_ != NULL){
      data_->Release();
    }
  }

  void set_phone(const std::string& phone){
    data_->phone_ = phone;
  }

  void set_vercode(const int32 vercode){
    data_->vercode_ = vercode;
  }

  void set_dead_line(const int64 dead_line){
    data_->dead_line_ = dead_line;
  }

  bool cmp_dead_line(){
    return (data_->dead_line_ + 120) > time(NULL)? true:false;
  }

  const std::string& phone() const {
    return data_->phone_;
  }

  const int32 vercode() const {
    return data_->vercode_;
  }

 private:
  class Data {
   public:
    Data()
        : refcount_(1)
       , dead_line_(0)
       , vercode_(0){
    }

    ~Data() {
    }
   public:
    int64       dead_line_;
    std::string phone_;
    int32       vercode_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }
    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }
   private:
    int refcount_;
  };

  Data* data_;
};

typedef RegiserVercode LoginVercode;

class UserInfo {
 public:
  UserInfo();
  UserInfo(const UserInfo& info);
  UserInfo& operator =(const UserInfo& info);

  ~UserInfo() {
    if (data_ != NULL){
      data_->Release();
    }
  }

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void set_phone(const std::string& phone){
    data_->phone_ = phone;
  }

  void set_uid(const int64 uid){
    data_->uid_ = uid;
  }

  void set_token(const std::string& token){
    data_->token_ = token;
  }

  const int64 uid() const {return data_->uid_;}

  const std::string& phone() const {return data_->phone_;}

  const std::string& token() const {return data_->token_;}

 private:
  class Data {
   public:
    Data()
        : refcount_(1)
       , uid_(0){
    }

    ~Data() {
    }
   public:
    int64       uid_;
    std::string phone_;
    std::string token_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }
    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }
   private:
    int refcount_;
  };

  Data* data_;
};

}

#endif /* USER_INFO_H_ */
