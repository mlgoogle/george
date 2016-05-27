// Copyright (c) 2016 The george Authors. All rights reserved.
// news_memery.h
// Created on: 2016年5月18日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_MEMERY_H_
#define PLUGINS_NEWS_CHECK_NEWS_MEMERY_H_

#include <stdio.h>
#include <string.h>

#include "../news_check/news_base.h"
#include "news_check/news_mysql.h"
#include "base/thread/base_thread_lock.h"

const int MAX_NEWS_COUNT = 100000;
namespace news {

class NewsMemery {
 public:
  static NewsMemery* GetInstance();

  int UpdateCacheNews();
  void InitNewMysql(NewsMysql* ptr);
  void NewsAnalyze(NewsMap* news_map);

  void GetRfNews(std::string key, int type);
 private:
  void DelNews();

  //更新新闻分类
  void UpdateNews(News newer, News older);

  //describe : 比较新闻关联信息
  //increase : 新增关联数据
  //decrease ： 减少的关联数据
  void CompareString(std::string newer, std::string older,
                     std::vector<std::string>* increase,
                     std::vector<std::string>* decrease);

  //
  void AddAndDelCom(RfNewsMap* map, std::vector<std::string>* increase,
                    std::vector<std::string>* decrease, int64 id);

  //向分类cache中添加新闻
  void SaveNewsCache(News news);

  //向分类cache中删除新闻
  void DelNewsCache(News news);

  bool RfMapAdds(RfNewsMap* map, std::string strs, int64 id);
  bool RfMapDel(RfNewsMap* map, std::string strs, int64 id);

  NewsMemery();
  ~NewsMemery();

 private:
  NewsMap news_map_;
  RfNewsMap st_map_;
  RfNewsMap in_map_;
  RfNewsMap se_map_;

  //lasttime : 当前内存中新闻的更新时间（上次更新新闻的时间）
  int64 lasttime_;

  //oldtime_ : 当前内存中最早的新闻
  int64 oldtime_;

  //由news_interface传入，该类不负责管理，只使用
  NewsMysql* news_mysql_;

  threadrw_t*  lock_;
 private:
  static NewsMemery* instance_;


};

}  // namespace news
#endif  // PLUGINS_NEWS_CHECK_NEWS_MEMERY_H_
