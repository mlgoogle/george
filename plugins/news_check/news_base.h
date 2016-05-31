// Copyright (c) 2016 The george Authors. All rights reserved.
// news_base.h
// Created on: 2016年5月19日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_BASE_H_
#define PLUGINS_NEWS_CHECK_NEWS_BASE_H_

#include <string.h>
#include <map>
#include <deque>

#include "../news_check/news.h"

typedef std::map<int64, news::News> NewsMap;
typedef std::map<int64, news::SimpleNews> SimpleMap;
typedef std::map<std::string, std::map<int64, int64> > RfNewsMap;
typedef std::map<std::string, std::deque<news::NewsCount*> > CountMap;


#endif  // PLUGINS_NEWS_CHECK_NEWS_BASE_H_
