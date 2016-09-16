// Copyright (c) 2016 The george Authors. All rights reserved.
// news_memery.cc
// Created on: 2016年5月18日.
// Author: Paco.


#include "news/news_memery.h"

#include <assert.h>

#include "pub/tools/tools.h"

#include "logic/logic_comm.h"

//base_logic::RLockGd lk(lock_);
//base_logic::WLockGd lk(lock_);
namespace news {

NewsMemery* NewsMemery::instance_;

NewsMemery::NewsMemery() {
  lasttime_ = 0;
  count_time_ = 0;
  seven_days_time_ = 0;
  news_mysql_ = NULL;
  InitThreadrw(&lock_);
}

NewsMemery::~NewsMemery() {
  DeinitThreadrw(lock_);
}

NewsMemery* NewsMemery::GetInstance() {
  if (NULL == instance_)
    instance_ = new NewsMemery();
  return instance_;
}

void NewsMemery::InitNewMysql(NewsMysql* ptr) {
  news_mysql_ = ptr;
}

int NewsMemery::UpdateCacheNews() {
  int err = 0;
  //最新的count条新闻
  do {
    NewsMap map;
    LOG_MSG2("exec sql before map.size(%d)", map.size());
    LOG_MSG2("lasttime_(%ld)", lasttime_);
    news_mysql_->QueryNews(lasttime_, EVERY_NEWS_COUNT, &map);
    if (map.size() <= 0) {
      break;
    }
    lasttime_ = tools::CurrentTime()*1000;
    LOG_MSG2("exec sql after map.size(%d)", map.size());
    NewsAnalyze(&map);
  } while (0);
  //最近7天的与股票相关的新闻
  do {
    //新的7天（过了一天）
    if (GetDaysMilliSecond(7) != seven_days_time_ && count_time_ != 0) {
      LOG_DEBUG("a day has gone!");
      UpAllSimpleMap();
      seven_days_time_ = GetDaysMilliSecond(7);
    }
    int64 time_now = tools::CurrentTime()*1000;;
    int64 time_old = 0;
    if (count_time_ == 0) {
      time_old = GetDaysMilliSecond(7);
      seven_days_time_ = time_old;
    } else {
      time_old = count_time_;
    }
    SimpleMap nmap;
    news_mysql_->QueryStNews(time_now, time_old, &nmap);
    NewsStAnalyze(&nmap);
    count_time_ = time_now;
  } while (0);
  return err;
}

void NewsMemery::UpAllSimpleMap() {
  base_logic::WLockGd lk(lock_);
  CountMap::iterator it = co_map_.begin();
  for (; it != co_map_.end(); ++it) {
    std::deque<NewsCount*>& deq = it->second;
    NewsCount* nnews = deq.back();
    deq.pop_back();
    if (nnews != NULL) {
      nnews->count_ = 0;
      nnews->nagative_ = 0;
      nnews->non_nagative_ = 0;
      deq.push_front(nnews);
    } else {
      LOG_ERROR("something is wrong");
    }
  }
}

void NewsMemery::NewsStAnalyze(SimpleMap* map) {
  base_logic::WLockGd lk(lock_);
  if (map->size() <= 0)
    return;
  SimpleMap::iterator it = map->begin();
  //之前已经统计过的新闻
  for (; it != map->end(); ++it) {
    SimpleNews news = it->second;
    SimpleMap::iterator it_cache = simple_map_.find(news.id());
    //新闻已经在内存中，需要比对更新
    if (it_cache != simple_map_.end()) {
      SimpleNews old_news = it_cache->second;
      UpdateNewsCount(news, old_news);
      simple_map_.insert(SimpleMap::value_type(news.id(), news));
    }
    else {
      simple_map_.insert(SimpleMap::value_type(news.id(), news));
      CountStNews(news);
    }
  }
}

void NewsMemery::UpdateNewsCount(SimpleNews new_sim, SimpleNews old_sim) {
  std::vector<std::string> decrease;
  std::vector<std::string> increase;
  std::vector<std::string> same;
  CompareString(new_sim.rf_stock(), old_sim.rf_stock(), &increase, &decrease,
                &same);
  std::vector<std::string>::iterator de_it = decrease.begin();
  for (; de_it != decrease.end(); de_it++) {
    CountMap::iterator it = co_map_.find(*de_it);
    if (it != co_map_.end()) {
      std::deque<NewsCount*>& deq = it->second;
      int32 pos = GetDaysToCur(new_sim.time());
      if (pos >=7 || pos < 0)
        continue;
      NewsCount* nnews = deq[pos];
      nnews->count_--;
      if (old_sim.sentiment() == 1)
        nnews->nagative_--;
      if (old_sim.sentiment() == 0)
        nnews->non_nagative_--;
    }
  }
  std::vector<std::string>::iterator in_it = increase.begin();
  for (; in_it != increase.end(); ++in_it) {
    CountMap::iterator it = co_map_.find(*in_it);
    if (it != co_map_.end()) {
      std::deque<NewsCount*>& deq = it->second;
      int32 pos = GetDaysToCur(new_sim.time());
      if (pos >=7 || pos < 0)
        continue;
      NewsCount* nnews = deq[pos];
      nnews->count_--;
      if (new_sim.sentiment() == 1)
        nnews->nagative_--;
      if (new_sim.sentiment() == 0)
        nnews->non_nagative_--;
    }
  }
  std::vector<std::string>::iterator sa_it = same.begin();
  for (; sa_it != same.end(); ++sa_it) {
    CountMap::iterator it = co_map_.find(*sa_it);
    if (it != co_map_.end()) {
      std::deque<NewsCount*>& deq = it->second;
      int32 pos = GetDaysToCur(new_sim.time());
      if (pos >=7 || pos < 0)
        continue;
      NewsCount* nnews = deq[pos];
      if (new_sim.sentiment() != old_sim.sentiment()) {
        if (new_sim.sentiment() == 1)
          nnews->nagative_++;
        if (new_sim.sentiment() == 0)
          nnews->non_nagative_++;
        if (old_sim.sentiment() == 0)
          nnews->non_nagative_--;
        if (old_sim.sentiment() == 1)
          nnews->nagative_--;
      }
    }
  }
}

void NewsMemery::CountStNews(SimpleNews news) {
  std::string stock = news.rf_stock();
  int64 time = news.time();
  int32 senti = news.sentiment();
  std::string::size_type pos;
  std::string pattern = ",";
  stock += pattern;
  int size = stock.size();
  for (int i = 0; i < size; i++) {
    pos = stock.find(pattern, i);
    if (pos < size) {
      std::string item = stock.substr(i , pos - i);
      CountMap::iterator it = co_map_.find(item);
      // 关系表中存在，则删除
      if (it != co_map_.end()) {
        std::deque<NewsCount*>& count_vec = it->second;
        int32 pos = GetDaysToCur(time);
        if (pos >= 7 || pos < 0)
           continue;
        NewsCount* nnews = count_vec[pos];
        nnews->count_++;
        if (senti == 1)
          nnews->nagative_++;
        if (senti == 0)
          nnews->non_nagative_++;
      } else {
        int32 pos = GetDaysToCur(time);
        if (pos >= 7 || pos < 0)
          continue;
        std::deque<NewsCount*> count_vec;
        Init7Vector(&count_vec);
        co_map_[item] = count_vec;
        NewsCount* nnews = count_vec[pos];
        nnews->count_++;
        if (senti == 1)
          nnews->nagative_++;
        if (senti == 0)
          nnews->non_nagative_++;
      }
      i = pos + pattern.size() - 1;
    }
  }
}

int32 NewsMemery::GetDaysToCur(int64 millitime) {
  return 6-(millitime - seven_days_time_)/(24*3600*1000);
}

void NewsMemery::Init7Vector(std::deque<NewsCount*>* deq) {
  assert(deq);
  for (int i = 0; i < deq->size(); i++) {
    NewsCount* news_count = deq->front();
    deq->pop_front();
    if (news_count != NULL) {
      delete news_count;
      news_count = NULL;
    }
  }
  for (int i = 0; i < 7; i++) {
    NewsCount* news_count = new NewsCount;
    news_count->count_ = 0;
    news_count->nagative_ = 0;
    news_count->non_nagative_ = 0;
    deq->push_back(news_count);
  }
}

//新闻解析
void NewsMemery::NewsAnalyze(NewsMap* map) {
  base_logic::WLockGd lk(lock_);
  if (map->size() <= 0)
    return;
  NewsMap::iterator it = map->begin();
  for (; it != map->end(); ++it) {
    News news = it->second;
    NewsMap::iterator it_cache = news_map_.find(news.id());
    //新闻已经在内存中，需要比对更新
    if (it_cache != news_map_.end()) {
      News old_news = it_cache->second;
      UpdateNews(news, old_news);
      news_map_.insert(NewsMap::value_type(news.id(), news));
    } else {
      if (news_map_.size() >= MAX_NEWS_COUNT) {
        LOG_DEBUG("news_map_.size() >= MAX_NEWS_COUNT, delnews");
        DelNews();
      }
      news_map_.insert(NewsMap::value_type(news.id(), news));
      SaveNewsCache(news);
    }
  }

}

void NewsMemery::UpdateNews(News newer, News older) {
  assert(newer.id() == older.id());
  std::vector<std::string> increase;
  std::vector<std::string> decrease;
  CompareString(newer.rf_stock(), older.rf_stock(), &increase, &decrease);
  AddAndDelCom(&st_map_, &increase, &decrease, newer.id());
  CompareString(newer.rf_industry(), older.rf_industry(), &increase, &decrease);
  AddAndDelCom(&in_map_, &increase, &decrease, newer.id());
  CompareString(newer.rf_section(), older.rf_section(), &increase, &decrease);
  AddAndDelCom(&se_map_, &increase, &decrease, newer.id());
}

void NewsMemery::AddAndDelCom(RfNewsMap* map, std::vector<std::string>* increase,
                  std::vector<std::string>* decrease, int64 id) {
  std::vector<std::string>::iterator de_it = decrease->begin();
  for (; de_it != decrease->end(); ) {
    RfNewsMap::iterator it = map->find(*de_it);
    if (it != map->end()) {
      std::map<int64, int64> tmap = it->second;
      std::map<int64, int64>::iterator tmap_it = tmap.find(id);
      if (tmap_it != tmap.end()) {
        tmap.erase(tmap_it);
      }
    }
    decrease->erase(de_it);
  }
  std::vector<std::string>::iterator in_it = increase->begin();
  for (; in_it != increase->end(); ) {
    RfNewsMap::iterator it = map->find(*in_it);
    if (it != map->end()) {
      std::map<int64, int64> tmap = it->second;
      std::map<int64, int64>::iterator tmap_it = tmap.find(id);
      if (tmap_it == tmap.end()) {
        tmap[id] = id;
      }
    }
    increase->erase(in_it);
  }

  assert(increase->size() == 0);
  assert(decrease->size() == 0);
}

void NewsMemery::CompareString(std::string newer, std::string older,
                   std::vector<std::string>* increase,
                   std::vector<std::string>* decrease,
                   std::vector<std::string>* same) {
  do {
    *decrease = tools::Split(older, ",");
    *increase = tools::Split(newer, ",");
    std::vector<std::string>::iterator de_it = decrease->begin();
    bool flag;
    for (; de_it != decrease->end(); ) {
      std::vector<std::string>::iterator in_it = increase->begin();
      flag = false;
      for (; in_it != increase->end(); ) {
        if (*in_it == *de_it) {
          if (same != NULL) {
            same->push_back(*in_it);
          }
          increase->erase(in_it);
          decrease->erase(de_it);
          flag = true;
          break;
        } else {
          ++in_it;
        }
      }
      if (!flag)
        ++de_it;
    }
  } while (0);

}

void NewsMemery::DelNews() {
  NewsMap::iterator it = news_map_.begin();
  for (int i = 0; i < 500 && it != news_map_.end(); i++) {
      News news = it->second;
      DelNewsCache(news);
      news_map_.erase(++it);
  }
}

void NewsMemery::DelNewsCache(News news) {
  std::string stocks = news.rf_stock();
  std::string industry = news.rf_industry();
  std::string session = news.rf_section();
  RfMapDel(&st_map_, stocks, news.id());
  RfMapDel(&in_map_, industry, news.id());
  RfMapDel(&se_map_, session, news.id());
}

void NewsMemery::SaveNewsCache(News news) {
  std::string stocks = news.rf_stock();
  std::string industry = news.rf_industry();
  std::string session = news.rf_section();
  RfMapAdds(&st_map_, stocks, news.id());
  RfMapAdds(&in_map_, industry, news.id());
  RfMapAdds(&se_map_, session, news.id());
}

bool NewsMemery::RfMapDel(RfNewsMap* map, std::string strs, int64 id) {
  if (strs.length() <= 0)
    return false;
  std::string::size_type pos;
  std::string pattern = ",";
  strs += pattern;
  int size = strs.size();
  std::map<int64, int64> temp_map;
  for (int i = 0; i < size; i++) {
    pos = strs.find(pattern, i);
    if (pos < size) {
      //item 股票-行业-板块
      std::string item = strs.substr(i , pos - i);
      RfNewsMap::iterator it = map->find(item);
      // 关系表中存在，则删除
      if(it != map->end()) {
        map->erase(it);
      }
      i = pos + pattern.size() - 1;
    }
  }
  return true;
}

bool NewsMemery::RfMapAdds(RfNewsMap* map, std::string strs, int64 id) {
  if (strs.length() <= 0)
    return false;
  std::string::size_type pos;
  std::string pattern = ",";
  strs += pattern;
  int size = strs.size();
  std::map<int64, int64> temp_map;
  for (int i = 0; i < size; i++) {
    pos = strs.find(pattern, i);
    if (pos < size) {
      //item 股票-行业-板块
      std::string item = strs.substr(i , pos - i);
      RfNewsMap::iterator it = map->find(item);
      if (it != map->end()) {
        std::map<int64, int64>& n_map = it->second;
        if (n_map.find(id) == n_map.end()) {
          n_map[id] = id;
        }
      } else {
        std::map<int64, int64> n_map;
        n_map[id] = id;
        map->insert(RfNewsMap::value_type(item, n_map));
      }
      i = pos + pattern.size() - 1;
    }
  }
  return true;
}

void NewsMemery::GetRfNews(std::string key, int type) {
  RfNewsMap::iterator it1 = st_map_.begin();
  RfNewsMap::iterator it2 = in_map_.begin();
  RfNewsMap::iterator it3 = se_map_.begin();
  std::string s;
  int64 n = -1;
  for(; it1 != st_map_.end(); ++it1) {
    s = it1->first;
    LOG_MSG2("key[%s]", s.c_str());
    std::map<int64, int64> tmap = it1->second;
    std::map<int64, int64>::iterator it11 = tmap.begin();
    for(; it11 != tmap.end(); ++it11) {
      n = it11->first;
      LOG_MSG2("id[%lld]",  n);
    }
  }
  for(; it2 != in_map_.end(); ++it2) {
    s = it2->first;
    LOG_MSG2("key[%s]", s.c_str());
    std::map<int64, int64> tmap = it2->second;
    std::map<int64, int64>::iterator it11 = tmap.begin();
    for(; it11 != tmap.end(); ++it11) {
      n = it11->first;
      LOG_MSG2("id[%lld]",  n);
    }
  }
  for(; it3 != se_map_.end(); ++it3) {
    s = it3->first;
    LOG_MSG2("key[%s]", s.c_str());
    std::map<int64, int64> tmap = it3->second;
    std::map<int64, int64>::iterator it11 = tmap.begin();
    for(; it11 != tmap.end(); ++it11) {
      n = it11->first;
      LOG_MSG2("id[%lld]",  n);
    }
  }
  return;
}

void NewsMemery::QueryNewsDetails(int64 nid, send::NewsList* out_list) {
  NewsMap::iterator it = news_map_.find(nid);
  if (it != news_map_.end()) {
    News news = news_map_[nid];
    send::SendNews* send_news = new send::SendNews();
    SetSendNews(send_news, news);
    out_list->set_news(send_news->get());
  } else {
    News news;
    news_mysql_->QueryNewsDetails(nid, &news);
    send::SendNews* send_news = new send::SendNews();
    SetSendNews(send_news, news);
    out_list->set_news(send_news->get());
  }
}

void NewsMemery::QueryCountNews(std::string stock, send::NewsList* out_list) {
  CountMap::iterator it =  co_map_.find(stock);
  if (it != co_map_.end()) {
    std::deque<NewsCount*>& deq = it->second;
    std::deque<NewsCount*>::iterator d_it;
    for (d_it = deq.begin(); d_it != deq.end(); d_it++) {
      NewsCount* nnews = *d_it;
      send::SendSt* send_news = new send::SendSt();
      send_news->set_count(nnews->count_);
      send_news->set_nagative(nnews->nagative_);
      send_news->set_non_nagative(nnews->non_nagative_);
      out_list->set_news(send_news->get());
    }
  } else {
    for(int i = 0; i < 7; i++) {
      send::SendSt* send_news = new send::SendSt();
      send_news->set_count(0);
      send_news->set_nagative(0);
      send_news->set_non_nagative(0);
      out_list->set_news(send_news->get());
    }
  }
}

void NewsMemery::QueryMemeryNews(int32 relate, std::string& key, int64 news_id,
                                 int32 news_count, send::NewsList* out_list) {
  LOG_DEBUG2("relate[%d], key[%s], id[%lld], count[%d]", relate, key.c_str(),
             news_id, news_count);
  base_logic::RLockGd lk(lock_);
  if (relate == 0) {
    QueryMapNews(st_map_, relate, news_id, key, news_count, out_list);
  } else if (relate == 1) {
    QueryMapNews(in_map_, relate, news_id, key, news_count, out_list);
  } else if (relate == 2) {
    QueryMapNews(se_map_, relate, news_id, key, news_count, out_list);
  }
}

void NewsMemery::QueryMapNews(RfNewsMap map, int32 relate, int64 id,
                              std::string& key, int32 count,
                              send::NewsList* out_list) {
  RfNewsMap::iterator it = map.find(key);
  int query_sql = false;
  if (it != map.end()) {
    LOG_DEBUG2("key[%s]find success", key.c_str());
    std::map<int64, int64>& rf_map = it->second;
    std::map<int64, int64>::iterator i_it = rf_map.find(id);
    LOG_DEBUG2("start to find newsid[%d]", id);
    if (i_it != rf_map.end() && i_it != rf_map.begin()) {
      i_it--;
      LOG_DEBUG2("find newsid[%d] success", id);
      for (int i = 0; i < count && i_it != rf_map.begin(); ++i, --i_it) {
        int64 id = i_it->first;
        LOG_DEBUG2("---find-id[%d]---", id);
        if (news_map_.find(id) != news_map_.end()) {
          News news = news_map_[id];
          send::SendNews* send_news = new send::SendNews();
          SetSendNews(send_news, news);
          out_list->set_news(send_news->get());
        }
      }
      if (out_list->size() < count) {
        int64 id = i_it->first;
        LOG_DEBUG2("---begin-find-id[%d]---", id);
        if (news_map_.find(id) != news_map_.end()) {
          News news = news_map_[id];
          send::SendNews* send_news = new send::SendNews();
          SetSendNews(send_news, news);
          out_list->set_news(send_news->get());
        }
      }
    } else {
       LOG_DEBUG2("find newsid[%d] failed", id);
      if (id <= 0) {
        //逆序遍历map 取出数据
        std::map<int64, int64>::reverse_iterator r_it = rf_map.rbegin();
        for (int i = 0; i < count && r_it != rf_map.rend(); i++, ++r_it) {
          int64 id = r_it->first;
          LOG_DEBUG2("---find-id[%d]---", id);
          LOG_DEBUG2("---news_map_.size[%d]---", news_map_.size());
          if (news_map_.find(id) != news_map_.end()) {
            LOG_DEBUG2("---%d---", 3);
            News news = news_map_[id];
            send::SendNews* send_news = new send::SendNews();
            SetSendNews(send_news, news);
            out_list->set_news(send_news->get());
          }
        }
      } else {
        query_sql = true;
      }
    }
  } else {
    query_sql = true;
  }
  if (query_sql) {
    LOG_DEBUG2("---%d---", 15);
    news_mysql_->QueryRfNews(id, count, relate, key, out_list);
  }
}

void NewsMemery::SetSendNews(send::SendNews* send_news, News news) {
  send_news->set_nid(news.id());
  send_news->set_type(news.type());
  send_news->set_time(news.time());
  send_news->set_sentiment(news.sentiment());
  send_news->set_title(news.title());
  send_news->set_url(news.url());
  send_news->set_summary(news.summary());
  send_news->set_stock(news.rf_stock());
  send_news->set_industry(news.rf_industry());
  send_news->set_section(news.rf_section());
  send_news->set_from(news.from());
}

//
int64 NewsMemery::GetDaysMilliSecond(int days) {
  if (days < 1)
    days = 1;
  int64 sec;
  struct tm timeTm;
  time_t timep;
  time(&timep);
  sec = timep;
  localtime_r(&timep, &timeTm);
  timeTm.tm_hour;
  timeTm.tm_min;
  timeTm.tm_sec;
  sec = sec - timeTm.tm_hour*3600 - timeTm.tm_min*60 - timeTm.tm_sec -
      24*3600*(days - 1);
  return sec*1000;
}

}  // namespace news
