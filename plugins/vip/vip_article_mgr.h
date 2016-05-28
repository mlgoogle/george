//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#ifndef GEORGE_VIP_ARTICLE_MGR_H__
#define GEORGE_VIP_ARTICLE_MGR_H__

#include "vip_db.h"
#include "vip_basic_info.h"
#include "storage/data_engine.h"
#include "basic/basictypes.h"
#include "thread/base_thread_lock.h"
#include "vip_basic_info.h"
#include <list>

typedef std::map<int64, vip_logic::ArticleInfo> ARTICLEINFO_MAP;
typedef std::list<vip_logic::ArticleInfo> ARTICLEINFO_LIST;
typedef std::vector<vip_logic::ArticleInfo>  ARTICLEINFO_VEC;
typedef std::map<int64, ARTICLEINFO_VEC>  VIPARTICLE_VEC;
namespace vip_logic {

class ArticleCache {
public:
	ARTICLEINFO_MAP    article_info_map_;
	ARTICLEINFO_LIST   article_info_list_;
	ARTICLEINFO_VEC    article_info_vec_;
	VIPARTICLE_VEC     vip_article_info_vec_;
	VIPARTICLE_VEC     vip_live_info_vec_;
};

class ArticleManager {
 public:
	ArticleManager();
	virtual ~ArticleManager();
 private:
	void Init();
 public:
	void Init(vip_logic::VIPDB* vip_db);
 public:
	bool GetArticleInfo(const int64 aid, vip_logic::ArticleInfo& article);
	bool GetArticleInfo(const int64* aid,
			std::map<int64,vip_logic::ArticleInfo>& map);

	bool GetNewArticle(std::list<vip_logic::ArticleInfo>& list,const int32 count);

	bool GetNewArticle(std::list<vip_logic::ArticleInfo>& list,const int32 pos,
			const int32 count);

	void SetVIPArticle(void);

	bool GetVIPArticle(const int64 vid,
			std::list<vip_logic::ArticleInfo>& list,const int32 pos,
			const int32 count,const int8 flag = 0);
 private:
	bool GetVIPArticleT(std::list<vip_logic::ArticleInfo>& list,const int32 pos,
			const int32 count, ARTICLEINFO_VEC& vec);

 private:
	struct threadrw_t*         lock_;
	ArticleCache*              article_cache_;
	vip_logic::VIPDB*          vip_db_;
};

class ArticleEngine {
 private:
	static ArticleManager*  article_mgr_;
	static ArticleEngine*   article_engine_;
	ArticleEngine() {}
	virtual ~ArticleEngine() {}
 public:
	static ArticleManager* GetArticleManager() {
		if (article_mgr_ == NULL)
			article_mgr_ = new ArticleManager();
		return article_mgr_;
	}

	static ArticleEngine* GetArticleEngine() {
		if (article_engine_ == NULL)
			article_engine_ = new ArticleEngine();
		return article_engine_;
	}

	static void FreeArticleManager() {
		delete article_mgr_;
	}

	static void FreeArticleEngine() {
		delete article_engine_;
	}
};
}
#endif
