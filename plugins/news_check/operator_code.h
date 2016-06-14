// Copyright (c) 2016 The george Authors. All rights reserved.
// opera_code.h
// Created on: 2016年5月30日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_OPERATOR_CODE_H_
#define PLUGINS_NEWS_CHECK_OPERATOR_CODE_H_

#include "pub/net/comm_head.h"

namespace news {

#define RELATE_NEWS_REQ   george_logic::NEWS_TYPE * 1000 + 1
#define RELATE_NEWS_RLY   george_logic::NEWS_TYPE * 1000 + 2

#define RELATE_COUNT_REQ  george_logic::NEWS_TYPE * 1000 + 3
#define RELATE_COUNT_RLY  george_logic::NEWS_TYPE * 1000 + 4

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_OPERATOR_CODE_H_
