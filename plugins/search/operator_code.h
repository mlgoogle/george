// Copyright (c) 2016 The george Authors. All rights reserved.
// opera_code.h
// Created on: 2016年5月30日.
// Author: Paco.
#ifndef PLUGINS_SEARCH_OPERATOR_CODE_H_
#define PLUGINS_SEARCH_OPERATOR_CODE_H_

#include "net/comm_head.h"

namespace search {

#define SEARCH_REQ   george_logic::SEARCH_TYPE * 1000 + 1
#define SEARCH_RLY   george_logic::SEARCH_TYPE * 1000 + 2

#define QUERY_SUBSCRIBE_REQ  george_logic::SEARCH_TYPE * 1000 + 3
#define QUERY_SUBSCRIBE_RLY  george_logic::SEARCH_TYPE * 1000 + 4

#define HOT_SUBSCRIBE_REQ george_logic::SEARCH_TYPE * 1000 + 5
#define HOT_SUBSCRIBE_RLY george_logic::SEARCH_TYPE * 1000 + 6

#define ALTER_SUBSCRIBE_REQ george_logic::SEARCH_TYPE * 1000 + 7
#define ALTER_SUBSCRIBE_RLY george_logic::SEARCH_TYPE * 1000 + 8
}  // namespace search



#endif  // PLUGINS_NEWS_CHECK_OPERATOR_CODE_H_
