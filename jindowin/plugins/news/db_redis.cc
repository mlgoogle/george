//  Copyright (c) 2015-2015 The db_redis.cc Authors. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <sstream>
#include "public/basic/radom_in.h"
#include "tools/tools.h"
#include "net/error_comm.h"
#include "net/http_data.h"
#include "news/news_logic.h"
#include "news/db_mysql.h"
#include "news/db_redis.h"

namespace news {
DbRedis::DbRedis() {
}

DbRedis::~DbRedis() {
}

void DbRedis::Dest() {
  base_dic::RedisPool::Dest();
}

void DbRedis::Init(std::list<base::ConnAddr> *const addrlist) {
  base_dic::RedisPool::Init(*addrlist);
}

bool SortByTime(const std::string& str1, const std::string& str2);
bool SortByTime(const std::string& str1, const std::string& str2) {
  return (str1.compare(str2) > 0);
}

int32 DbRedis::GetAllNews(int64 count, StockNewsSend* send_news) {
  int32 err = 0;
  int32 page_index = 0;
  int64 page = count;
  bool r = true;
  if (count < MIN_COUNT)
    count = MIN_COUNT;
  do {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
    if (redis == NULL) {
      LOG_ERROR("Redis GetConnection Error");
      err = REIDS_CONNECT_ERROR;
      break;
    }
    int64 unix_time;
    std::string filed;
    int32 suc = 0;
    for (int32 i = 0; i < 3; i++) {
      unix_time = tools::CurrentTime() - DAY_TIME_SEC*i;
      filed = tools::TimeFormat(unix_time, "News_%04d%02d%02d");
      GetRationNewsByPage(page_index, page, send_news, filed, redis, &suc);
      if (page_index >(page*PAGE_NEWS_NUM -1)) {
        break;
      }
    }
  } while (0);
  return err;
}

std::string DbRedis::GetRationNewsId(std::string hy_name, std::string filed,
    base_storage::DictionaryStorageEngine* redis) {
  bool r = true;
  std::string key;
  size_t key_len;
  key = hy_name;
  key_len = key.length();
  char *val = NULL;
  size_t val_len = 0;
  r = redis->GetHashElement(filed.c_str(), key.c_str(),
      key_len, &val, &val_len);
  LOG_DEBUG2("GetRationNewsId %s %s：%s", filed.c_str(), key.c_str(), val);
  if (!r || val == NULL) {
    return "";
  }
  std::string value;
  if (NULL != val) {
    value = val;
    free(val);
    val = NULL;
  }
  return value;
}

void DbRedis::SetSendByPage(std::string filed, std::string value, \
    StockNewsSend* send_news, int32 page) {
  if (value.length() < 1) {
    return;
  }
  std::vector<std::string> news_vector = tools::Split(value, ",");
  int32 total = static_cast<int32>(news_vector.size());
  if (0 >= page) {
    page = total/PAGE_NEWS_NUM;
    if (0 >= page) {
      page = 1;
    }
    send_news->SetPageCount(page);
  }
  std::sort(news_vector.begin(), news_vector.end(), SortByTime);
  // 按页获取
  int32 page_index = 0;
  std::vector<std::string> vec_news_id;
  for (std::vector<std::string>::reverse_iterator it = news_vector.rbegin();
      it != news_vector.rend(); ++it) {
    // judge index
    if ((page-1)*PAGE_NEWS_NUM -1 >= page_index) {
      page_index++;
      continue;
    }
    if (page_index > (page*PAGE_NEWS_NUM-1)) {
      break;
    }
    page_index++;
    vec_news_id.push_back(*it);
  }
  // filter news 
  FilterNews(&vec_news_id);
  for (int32 i = vec_news_id.size()-1; i >= 0; i--) {
    if (send_news->IsAdded(vec_news_id[i])) {
      continue;
    }
    std::string res;
    if (QueryNewsById(filed, vec_news_id[i], &res) == 0) {
      std::string error_str;
      int error_code = 0;
      scoped_ptr<base_logic::ValueSerializer>engine\
      (base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &res));
      NetBase* v = reinterpret_cast<NetBase*>(
          engine->Deserialize(&error_code, &error_str));
      if (NULL == v) {
        continue;
      } else {
        // append news summary
        std::string news_summary, news_url;
        v->GetString(L"url", &news_url);
        if (news_url.length() > 0) {
          news::DbMysql::QueryNewsSummary(news_url, news_summary);
        }
        if (news_summary.length() > NEWS_SUMMARY_LENGTH_MAX) {
          TruncatedUTF8String(&news_summary, NEWS_SUMMARY_LENGTH_MAX);
        }
        v->SetString(L"detail", news_summary);
      }
      std::string con;
      v->GetString(L"url", &con);
      std::string from;
      v->GetString(L"from", &from);
      if (from.length() <= 0)
        v->SetString("from", "");
      ConvertNewsDateTimeFmt(v);
      AppendNewsData(v);
      send_news->AddNews(v);
      send_news->AddNewsId(news_vector[i]);
    }
  }
}
/**
 *
 */
int32 DbRedis::GetStockNews(std::string stock, int64 count,
                            StockNewsSend* send_news) {
  int32 err = 0;
  bool r = true;
  do {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
    if (redis == NULL) {
      LOG_ERROR("Redis GetConnection Error");
      err = REIDS_CONNECT_ERROR;
      break;
    }
    int64 unix_time;
    std::string filed;
    std::string value;
    for (int32 i = 0; i < 3; i++) {
      unix_time = tools::CurrentTime() - DAY_TIME_SEC*i;
      filed = tools::TimeFormat(unix_time, "Stock_%04d%02d%02d");
      value = GetRationNewsId(stock, filed, redis);
      if (value.length() > 0)
        break;
    }
    filed = tools::TimeFormat(unix_time, "News_%04d%02d%02d");
    SetSendByPage(filed, value, send_news, count);
  } while (0);
  return err;
}

int32 DbRedis::GetHyNews(std::string hy_name, int64 count,
                         StockNewsSend* send_news) {
  int32 err = 0;
  bool r = false;
  do {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
    if (redis == NULL) {
      LOG_ERROR("Redis GetConnection Error");
      err = REIDS_CONNECT_ERROR;
      break;
    }
    int64 unix_time;
    std::string filed;
    std::string value;
    for (int32 i = 0; i < 3; i++) {
      unix_time = tools::CurrentTime() - DAY_TIME_SEC*i;
      filed = tools::TimeFormat(unix_time, "Industry_%04d%02d%02d");
      value = GetRationNewsId(hy_name, filed, redis);
      if (value.length() > 0)
        break;
    }
    filed = tools::TimeFormat(unix_time, "News_%04d%02d%02d");
    SetSendByPage(filed, value, send_news, count);
  } while (0);
  return err;
}

int32 DbRedis::GetGnNews(std::string gn_name, int64 count,
                         StockNewsSend* send_news) {
  int32 err = 0;
  bool r = false;
  do {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
    if (redis == NULL) {
      LOG_ERROR("Redis GetConnection Error");
      err = REIDS_CONNECT_ERROR;
      break;
    }
    int64 unix_time;
    std::string filed;
    std::string value;
    std::string final_value;
    std::vector<std::string> gn_vector;
    if (gn_name.length() > 0)
      gn_vector = tools::Split(gn_name, ",");
    for (int32 j = 0; j < gn_vector.size(); j++) {
      value = "";
      for (int32 i = 0; i < 3; i++) {
        unix_time = tools::CurrentTime() - DAY_TIME_SEC*i;
        filed = tools::TimeFormat(unix_time, "Section_%04d%02d%02d");
        value = GetRationNewsId(gn_vector[j], filed, redis);
        if (value.length() > 0) {
          filed = tools::TimeFormat(unix_time, "News_%04d%02d%02d");
          break;
        }
      }
      if (value.length() >0) {
        SetSendByPage(filed, value, send_news, count);
      }
      if (count <= 0)
        break;
    }
  } while (0);
  return err;
}

int32 DbRedis::GetYbNews(std::string yb_name, int64 count,
                         StockNewsSend* send_news) {
  int32 err = 0;
  do {
  } while (0);
  return err;
}

int32 DbRedis::QueryNewsById(std::string filed, std::string news_id,
                             std::string* news_json) {
  int err = 0;
  bool r = true;
  do {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
    if (redis == NULL) {
      LOG_ERROR("Redis GetConnection Error");
      err = REIDS_CONNECT_ERROR;
      break;
    }
    size_t key_len;
    key_len = news_id.length();
    char *val = NULL;
    size_t val_len = 0;
    r = redis->GetHashElement(filed.c_str() , news_id.c_str(), \
        key_len, &val, &val_len);
    if (r) {
      LOG_DEBUG2("QueryNewsById%s：%s\n", news_id.c_str(), val);
      *news_json = val;
    } else {
      err = EXEC_REDIS_ERROR;
      break;
    }
    if (NULL != val)
      free(val);
  } while (0);
  return err;
}

int32 DbRedis::AterNews(int64 date, std::string news_id, int64 commit_type,
                        StockNewsSend* send_news, int64 user_id) {
  int32 err = 0;
  bool r = true;
  do {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
    if (redis == NULL) {
      LOG_ERROR("Redis GetConnection Error");
      err = REIDS_CONNECT_ERROR;
      break;
    }
    size_t key_len;
    key_len = news_id.length();
    char *val = NULL;
    size_t val_len = 0;
    std::stringstream ss;
    ss << "News_" << date/NUM_DATE_DIV;
    std::string filed = ss.str();
    r = redis->GetHashElement(filed.c_str(), news_id.c_str(), key_len, \
        &val, &val_len);
    if (r) {
      LOG_DEBUG2("QueryNewsById：%s\n", val);
      std::string json_str = val;
      std::string error_str;
      int error_code = 0;
      scoped_ptr<base_logic::ValueSerializer>engine\
      (base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &json_str));
      NetBase* v = reinterpret_cast<NetBase*>(
          engine->Deserialize(&error_code, &error_str));
      int64 count = 0;
      std::wstring str_k;

      // time format
      std::stringstream os;
      os << date;
      std::string str_num;
      os >> str_num;
      v->SetString(L"time", str_num);
      news_logic::CommentType etype = CheckUserComment(user_id, news_id,
          TimeNum2TimeFormat(date));
      if (news_logic::ECOMMENT_UNCOMMENT == etype) {
        // alter news
        if (news_logic::ECOMMENT_UP == commit_type)
          str_k = L"up";
        if (news_logic::ECOMMENT_DOWN == commit_type)
          str_k = L"down";
        r = v->GetBigInteger(str_k, &count);
        if (!r)
          break;
        count++;
        v->SetBigInteger(str_k, count);

        std::string out;
        scoped_ptr<base_logic::ValueSerializer>engine_out \
          (base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &out));
        engine_out->Serialize(*v);
        r = redis->SetHashElement(filed.c_str(), news_id.c_str(), \
                                  key_len, out.c_str(), out.length());
        // save comment
        SaveUserComment(user_id, news_id, TimeNum2TimeFormat(date), \
            static_cast<news_logic::CommentType>(commit_type));
        v->SetBigInteger(L"comment_recored", \
            static_cast<news_logic::CommentType>(commit_type));
      } else {
        v->SetBigInteger(L"comment_recored", etype);
      }
      v->SetString(L"time", TimeNum2TimeFormat(date));
      v->SetString(L"detail", "");
      AppendNewsData(v);
      send_news->AddNews(v);
    } else {
      err = EXEC_REDIS_ERROR;
      break;
    }
    if (NULL != val)
      free(val);
  } while (0);
  return err;
}

std::string DbRedis::TimeNum2TimeFormat(int64 time_num) {
  // 兼容2016-01-29 09:23:01之前的时间格式
  // 之前的时间没有精确到分和秒
  // defalut value
  int year = 2016;
  int month = 1;
  int day = 29;
  int hour = 18;
  int minitue = 3;
  int second = 25;
  second = time_num % 100;
  time_num = time_num / 100;
  minitue = time_num % 100;
  time_num = time_num / 100;
  hour = time_num % 100;
  time_num = time_num / 100;
  day = time_num % 100;
  time_num = time_num / 100;
  month = time_num % 100;
  time_num = time_num / 100;
  year = time_num % 10000;
  char buffer[22] = "";
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d", \
           year, month, day, hour, minitue, second);
  return buffer;
}

std::string DbRedis::TimeFormat2TimeNumStr(std::string timeformat) {
  int64 num = 0;
  int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
  sscanf(timeformat.c_str(), "%d-%d-%d %d:%d:%d", \
         &year, &month, &day, &hour, &minute, &second);
  char buffer[22] = "";
  snprintf(buffer, sizeof(buffer), "%04d%02d%02d%02d%02d%02d", \
           year, month, day, hour, minute, second);
  return buffer;
}

// get hashtable all key fields
bool DbRedis::GetHashKeysFileds(std::string table_name, \
                            std::vector<std::string> & out_field_list) {
  std::string redis_command = "hkeys ";
  redis_command.append(table_name);
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine* redis_auto = \
      auto_engine.GetDicEngine();
  base_storage::RedisStorageEngineImpl *redis = dynamic_cast< \
    base_storage::RedisStorageEngineImpl *>(redis_auto);
  if (redis == NULL) {
    LOG_ERROR("Redis GetConnection Error");
    return false;
  }
  // new interface [GetHashKeys]
  std::list<std::string> list_val;
  redis->GetHashKeys(table_name.c_str(), table_name.length(), list_val);
  for (std::list<std::string>::iterator it = list_val.begin(); \
      it != list_val.end(); ++it) {
    out_field_list.push_back(*it);
  }
  LOG_DEBUG2("---->out_field_list.size = %d", (int)out_field_list.size());
  return true;
}

int32 DbRedis::GetRationNewsByPage(int32 &page_index, int32 page,
    StockNewsSend* send_news, std::string filed,
    base_storage::DictionaryStorageEngine* redis, int32* suc) {
  std::string key;
  if (page_index > (page*PAGE_NEWS_NUM -1)) {
      return 0;
  }
  char *val = NULL;
  size_t val_len = 0;
  size_t key_len;
  key_len = key.length();
  bool r = true;
  std::list<std::string> list;
  std::vector<std::string> vec_news_id;
  GetHashKeysFileds(filed, vec_news_id);
  std::sort(vec_news_id.begin(), vec_news_id.end(), SortByTime);
  int tmecount = 0;
  std::vector<std::string> vec_news_final_id;
  for (std::vector<std::string>::reverse_iterator it = vec_news_id.rbegin();
      it != vec_news_id.rend(); ++it) {
    // judge index
    if ((page-1)*PAGE_NEWS_NUM -1 >= page_index) {
      page_index++;
      continue;
    }
    if (page_index > (page*PAGE_NEWS_NUM -1)) {
      break;
    }
    page_index++;
    /*
    std::string news_content;
    LOG_DEBUG2("[sorted newsid]:%s\n", (*it).c_str());
    QueryNewsById(filed, *it, &news_content);
    list.push_back(news_content);
    */
    vec_news_final_id.push_back(*it);
  }
  // filter news 
  FilterNews(&vec_news_final_id);
  for (std::vector<std::string>::iterator it = vec_news_final_id.begin();
      it != vec_news_final_id.end(); ++it) {
    std::string news_content;
    LOG_DEBUG2("[sorted newsid]:%s\n", (*it).c_str());
    QueryNewsById(filed, *it, &news_content);
    list.push_back(news_content);
  }

  LOG_DEBUG2("GetRationNewsByPage, news_num:%d, filed:%s", \
              list.size(), filed.c_str());
  if (r) {
    std::list<std::string>::reverse_iterator it = list.rbegin();
    for (; it != list.rend(); it++) {
      std::string res = *it;
      std::string error_str;
      int error_code = 0;
      scoped_ptr<base_logic::ValueSerializer> engine\
        (base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &res));
      NetBase* v = reinterpret_cast<NetBase*>(
          engine->Deserialize(&error_code, &error_str));
      if (NULL == v) {
        continue;
      } else {
        // append news summary
        std::string news_summary, news_url;
        v->GetString(L"url", &news_url);
        if (news_url.length() > 0) {
          news::DbMysql::QueryNewsSummary(news_url, news_summary);
        }
        if (news_summary.length() > NEWS_SUMMARY_LENGTH_MAX) {
          TruncatedUTF8String(&news_summary, NEWS_SUMMARY_LENGTH_MAX);
        }
        v->SetString(L"detail", news_summary);
      }
      ConvertNewsDateTimeFmt(v);
      AppendNewsData(v);
      send_news->AddNews(v);
      (*suc)++;
    }
  }
  return 0;
}

int32 DbRedis::GetNewsTotalSize(uint64 starttime, \
    std::vector<int32> &vec_day_news_size) {
  int32 total = 0;
  vec_day_news_size.clear();
  std::string table_name;
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
  if (redis == NULL) {
    LOG_ERROR("Redis GetConnection Error");
    return 0;
  }
  // 这里有三次查询，用管道命令一次完成提升效率
  std::list<std::string> cmd_list;
  std::list<base_storage::CommandReply*> reply_list;
  std::string cmd;
  for (int i = 0; i < 3; ++i) {
    int64 time_unix = starttime - DAY_TIME_SEC*i;
    cmd = "hlen ";
    cmd.append(tools::TimeFormat(time_unix, "News_%04d%02d%02d"));
    cmd_list.push_back(cmd);
    cmd.clear();
  }
  redis->DoCommands(cmd_list, reply_list);
  // LOG_DEBUG2("---->DoCommands, replysize:%u", reply_list.size());
  for (std::list<base_storage::CommandReply*>::iterator it = reply_list.begin();
      it != reply_list.end();
      ++it) {
    base_storage::CommandReply *reply = *it;
    if (NULL != reply && base_storage::CommandReply::REPLY_INTEGER) {
       base_storage::IntegerReply *int_reply = \
          dynamic_cast<base_storage::IntegerReply *>(reply);
       if (NULL != int_reply) {
         total += int_reply->value;
         vec_day_news_size.push_back(int_reply->value);
       }
    } else {
      total += 0;
      vec_day_news_size.push_back(0);
      LOG_DEBUG("reply error!!");
    }
    if (NULL != reply) {
      reply->Release();
    }
  }
  return total;
}

int32 DbRedis::GetAllPageNews(int64 page, StockNewsSend* send_news) {
  int32 err = 0;
  int32 page_index = 0;
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine* redis = auto_engine.GetDicEngine();
  if (redis == NULL) {
    LOG_ERROR("Redis GetConnection Error");
    err = REIDS_CONNECT_ERROR;
    return err;
  }
  LOG_DEBUG("[GetAllPageNews]");
  std::vector<int32> vec_news_day_size;
  int32 total = GetNewsTotalSize(tools::CurrentTime(), vec_news_day_size);
  if (0 >= page) {
    page = total/PAGE_NEWS_NUM;
    send_news->SetPageCount(page);
    LOG_DEBUG2("-->SetPageCount:%lld", page);
  }
  int64 now_time = tools::CurrentTime();
  std::string table_name;
  int32 success = 0;
  int32 news_num_need = (page-1)*PAGE_NEWS_NUM;
  int32 day_count = 1;
  int32 sum_size = 0;
  for (std::vector<int32>::reverse_iterator it = vec_news_day_size.rbegin();
      it != vec_news_day_size.rend();
      ++it, day_count++) {
    sum_size += *it;
    if (sum_size > news_num_need) {
      break;
    } else {
        page_index += *it;
    }
  }
  int64 query_start_unixtime = now_time - \
        (vec_news_day_size.size() - day_count)*DAY_TIME_SEC;
  // query page news
  for (int64 unix_time = query_start_unixtime; \
      unix_time <= now_time; unix_time+=DAY_TIME_SEC) {
    table_name = tools::TimeFormat(unix_time, "News_%04d%02d%02d");
    LOG_DEBUG2("Query news table:%s, page_index:%lld", \
        table_name.c_str(), page_index);
    GetRationNewsByPage(page_index, page, send_news, \
        table_name, redis, &success);
    if (page_index > (page*PAGE_NEWS_NUM -1)) {
      break;
    }
  }
  return err;
}

int32 DbRedis::SaveNews(const std::string& news_table,
    const std::string &news_id, \
    const std::string &news_json_str) {
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();
  if (NULL == redis) {
    return 0;
  }
  return redis->SetHashElement(news_table.c_str(), news_id.c_str(), \
      news_id.length(), news_json_str.c_str(), news_json_str.length());
}

news_logic::CommentType DbRedis::CheckUserComment(int64 user_id, \
    std::string news_id, std::string news_date_fmt) {
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();
  if (NULL == redis) {
    return news_logic::ECOMMENT_UNCOMMENT;
  }
  std::string date_numstr = TimeFormat2TimeNumStr(news_date_fmt);
  date_numstr.erase(DATETIME_STRING_TIME_POS, std::string::npos);
  std::stringstream ss;
  ss << "NewsCommentRecord_" << date_numstr;
  std::string table_name, key;
  ss >> table_name;
  ss.clear();
  ss << user_id;
  ss >> key;
  ss.clear();
  char * val = NULL;
  size_t len = 0;
  if (!redis->GetHashElement(table_name.c_str(), \
        key.c_str(), key.length(), &val, &len)) {
    return news_logic::ECOMMENT_UNCOMMENT;
  }
  if (val != NULL) {
    std::string news_id_commented_val = val;
    free(val), val = NULL;
    std::vector<std::string> vec_news_id = \
                             tools::Split(news_id_commented_val, ",");
    std::vector<std::string>::iterator it = std::find(vec_news_id.begin(), \
        vec_news_id.end(), news_id + "-1");
    if (it != vec_news_id.end()) {
      return news_logic::ECOMMENT_UP;
    } else {
      it = find(vec_news_id.begin(), vec_news_id.end(), news_id + "-2");
      if (it != vec_news_id.end()) {
        return news_logic::ECOMMENT_DOWN;
      }
    }
  }
  return news_logic::ECOMMENT_UNCOMMENT;
}

bool DbRedis::SaveUserComment(int64 user_id, std::string news_id, \
    std::string news_date_fmt, \
    news_logic::CommentType etype) {
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();
  if (NULL == redis) {
    return news_logic::ECOMMENT_UNCOMMENT;
  }
  std::string date_numstr = TimeFormat2TimeNumStr(news_date_fmt);
  date_numstr.erase(DATETIME_STRING_TIME_POS, std::string::npos);
  std::stringstream ss;
  ss << "NewsCommentRecord_" << date_numstr;
  std::string table_name, key;
  ss >> table_name;
  ss.clear();
  ss << user_id;
  ss >> key;
  ss.clear();
  ss << news_id << "-" << static_cast<int>(etype);
  std::string news_id_comment;
  ss >> news_id_comment;
  ss.clear();
  char *val = NULL;
  size_t len = 0;
  if (!redis->GetHashElement(table_name.c_str(), key.c_str(), \
        key.length(), &val, &len)) {
    // 判断哈希表是否存在
    base_storage::CommandReply* cmd_repley = \
                redis->DoCommandV("exists %s", table_name.c_str());
    if (cmd_repley->type == base_storage::CommandReply::REPLY_INTEGER) {
       base_storage::IntegerReply *int_rep = \
               dynamic_cast<base_storage::IntegerReply*>(cmd_repley);
       if (NULL != int_rep && 0 == int_rep->value) {
          // 添加记录并设置过期时间
          redis->AddHashElement(table_name.c_str(), key.c_str(), key.length(),
              news_id_comment.c_str(), news_id_comment.length());
          std::stringstream ss_cmd;
          ss_cmd << "expire "<< table_name<< " "<< (3*DAY_TIME_SEC);
          redis->DoCommand(ss_cmd.str().c_str());
          LOG_DEBUG2("设置过期时间:%s", ss_cmd.str().c_str());
          return true;
       }
    }
  }
  std::string news_comment_list;
  if (NULL != val) {
    news_comment_list = val;
    free(val), val = NULL;
    news_comment_list += ",";
    news_comment_list += news_id_comment;
  } else {
    news_comment_list = news_id_comment;
  }
  return redis->SetHashElement(table_name.c_str(), \
      key.c_str(), key.length(), \
      news_comment_list.c_str(), news_comment_list.length());
}

void DbRedis::ConvertNewsDateTimeFmt(NetBase *news_kv) {
  if (NULL == news_kv) {
    return;
  }
  int64 timenum = 0;
  std::string time_str;
  news_kv->GetString(L"time", &time_str);
  std::stringstream os;
  os << time_str;
  os >> timenum;
  std::string timestr = TimeNum2TimeFormat(timenum);
  news_kv->SetString(L"time", timestr);
}

void DbRedis::AppendNewsData(NetBase *news_kv) {
  if (NULL == news_kv) {
    return;
  }
  static char* news_type_name[] = {
    "新闻", "新闻", "快讯", "自媒体"
  };
  static char* news_default_img[] = {
   "http://jindowin.iwookong.com/imgs/news_default_2.png",
   "http://jindowin.iwookong.com/imgs/news_default_2.png",
   "http://jindowin.iwookong.com/imgs/news_default_3.png",
   "http://jindowin.iwookong.com/imgs/news_default_1.png"
  };
  // news type conversion
  int64 news_type = 0;
  news_kv->GetBigInteger(L"type", &news_type);
  if (news_type >= sizeof(news_type_name)/sizeof(char*)) {
    news_type = 0;
  }
  std::string tmpstr = news_type_name[news_type];
  news_kv->SetString(L"type", tmpstr);
  // img url
  tmpstr.clear();
  news_kv->GetString(L"id", &tmpstr);
  std::string imgsrc;
  if (!GetNewsImg(tmpstr, imgsrc)) {
    imgsrc = news_default_img[news_type];    
  }
  news_kv->SetString(L"imgsrc", imgsrc);
  // 转发次数
  int64 transmitcount = 0;
  news_kv->GetBigInteger(L"transmit_count", &transmitcount);
  if (0 == transmitcount) {
    news_kv->SetBigInteger(L"transmit_count", transmitcount);
  }
}

bool DbRedis::GetNewsImg(std::string news_id, std::string & out_img) {
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();
  if (NULL == redis || news_id.length() < 1) {
    return false;
  }
  news_id.erase(0, 7);
  LOG_DEBUG2("GetNewsImg,news_id=%s", news_id.c_str());
  // read news image url
  int64 curr_time = tools::CurrentTime();
  int64 unix_time = 0;
  std::string table_name;
  for (int32 i = 0; i < 3; i++) {
    unix_time = curr_time - DAY_TIME_SEC*i;
    table_name = tools::TimeFormat(unix_time, "img_%04d%02d%02d");
    size_t len = 0;
    char *val = NULL;
    redis->GetHashElement(table_name.c_str(), news_id.c_str(), \
        news_id.length(), &val, &len);
    if (val != NULL && len > 0) {
      out_img = val;
      free(val), val = NULL;
      LOG_DEBUG2("[GetNewsImg], news_id:%s, img:%s", news_id.c_str(), \
          out_img.c_str());
      return true;
    }
  }
  return false;
}

// 读取情感值
bool DbRedis::GetSentimentValue(news_logic::SentimentType etype, \
    std::string key_field, \
    std::string *out_val_ptr) {
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();
  if (NULL == redis) {
    return false;
  }
  static char * senti_table[] = {"stock_sentiment", \
  "industry_sentiment", "section_sentiment"};
  std::string table_name = senti_table[etype];
  char* val = NULL;
  size_t len = 0;
  redis->GetHashElement(table_name.c_str(), key_field.c_str(), \
      key_field.length(), &val, &len);
  if (NULL == val || 0 == len) {
    (*out_val_ptr).append("0.0,0.0");  // default value
    return false;
  }
  *out_val_ptr = val;
  free(val);
  return true;
}

// 截取utf8字符串
bool DbRedis::TruncatedUTF8String(std::string* src, size_t len) {
  if (NULL == src) {
    return false;
  }
  int32 src_len = src->length();
  if (len >= src_len) {
    return true;
  }
  std::string & src_ref = *src;
  for (int32 indx = len; indx > 0; indx--) {
    unsigned char tmp_val = src_ref[indx];
    if (UTF8_SECOND_VALUE_MIN <= tmp_val && tmp_val < UTF8_FIRST_VALUE_MIN) {
      continue;
    } else if (tmp_val >= UTF8_FIRST_VALUE_MIN) {
      src_ref.erase(indx, std::string::npos);     
      break;
    } else if (tmp_val <= CHAR_MAX_VALUE) {
      break;
    }
  }
  return true;
}

// 过滤相同内容的新闻
void DbRedis::FilterNews(std::vector<std::string> * vec_news_ptr) {
  if (NULL == vec_news_ptr) {
    return; 
  }
  LOG_DEBUG("FilterNews");
  std::vector<std::string> vec_tem = *vec_news_ptr;
  vec_news_ptr->clear();
  for (std::vector<std::string>::iterator it = vec_tem.begin();
    it != vec_tem.end(); ++it) {
    int comparelen = it->length() -7;
    if (comparelen < 0) {
      continue;
    }
    bool existed = false;
    for (size_t i = 0; i < vec_news_ptr->size(); ++i) {
      if (0 == vec_news_ptr->at(i).compare(7, comparelen,
          *it, 7, comparelen)) {
            break;
            existed = true;
          }
    }
    if (!existed) {
     vec_news_ptr->push_back(*it);
    }
  }
}

}  //  namespace news
