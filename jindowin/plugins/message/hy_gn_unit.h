// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_HY_UNIT_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_HY_UNIT_H_

#include <string>
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "basic/basictypes.h"

class HyGnUnit {
 public:
  HyGnUnit(){}
  HyGnUnit(const std::string &code, const std::string &name, \
         double vp, double vip, \
         double sp, double sip, \
         double fp, double fip) {
    code_ = code; name_ = name;
    vp_ = vp; vip_ = vip;
    sp_ = sp; sip_ = sip;
    fp_ = fp; fip_ = fip;
  }

  virtual base_logic::DictionaryValue* Release() {
    scoped_ptr<base_logic::DictionaryValue> dict(\
			   new base_logic::DictionaryValue());

    dict->SetString(L"code", code_);
    dict->SetString(L"name", name_);

    dict->SetReal("visit_percent", vp_);
    dict->SetReal("visit_add", vip_);

    dict->SetReal("search_percent", sp_);
    dict->SetReal("search_add", sip_);

    dict->SetReal("follow_percent", fp_);
    dict->SetReal("follow_add", fip_);
   
    return dict.release();
  }

  std::string code_;
  std::string name_;

  double vp_;
  double vip_;

  double sp_;
  double sip_;

  double fp_;
  double fip_;
};

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_HY_UNIT_H_
