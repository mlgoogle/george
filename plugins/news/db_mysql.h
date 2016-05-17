//  Copyright (c) 2015-2015 The db_mysql.h Authors. All rights reserved.

#ifndef _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_DB_MYSQL_H_
#define _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_DB_MYSQL_H_

#include <vector>
#include <list>
#include <string>
#include "public/basic/basic_info.h"

#include "news/news_base.h"
#include "news/news_packet.h"

namespace news {
class DbMysql {
 public:
    DbMysql();
    ~DbMysql();

    static void Init(std::list<base::ConnAddr>* const addrlist);
    static void Dest();

 public:
    static bool QueryStockHy(StringMap* hy);
    static bool QueryStockGn(StringMap* gn);
    static int32 GetNewsAffect(int64 type, \
        std::string name, StockNewsSend* send_news, \
        std::string starttime, std::string endtime);
    static bool QueryStockName(StringMap* stock_map);
    static std::vector<std::string> GetUserSubscribe(int64 user_id, \
        int get_type);
    static bool QueryNewsSummary(std::string url, std::string & out_result);
};

}  // namespace news
#endif  // _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_DB_MYSQL_H_
