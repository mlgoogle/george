#include "vip_article_mgr.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include "thread/base_thread_lock.h"
#include <algorithm>

namespace vip_logic {

ArticleManager* ArticleEngine::article_mgr_ = NULL;
ArticleEngine* ArticleEngine::article_engine_ = NULL;

ArticleManager::ArticleManager()
:vip_db_(NULL) {
	article_cache_ = new ArticleCache();
	Init();
}

ArticleManager::~ArticleManager() {
	DeinitThreadrw(lock_);
}

void ArticleManager::Init() {
	InitThreadrw(&lock_);
}

void ArticleManager::Init(vip_logic::VIPDB* vip_db) {
	vip_db_ = vip_db;
	vip_db_->FectchArticleInfo(article_cache_->article_info_map_,
			article_cache_->article_info_list_,article_cache_->article_info_vec_);
	std::sort(article_cache_->article_info_vec_.begin(),
			article_cache_->article_info_vec_.end(),vip_logic::ArticleInfo::cmp);
	article_cache_->article_info_list_.sort(vip_logic::ArticleInfo::cmp);

}

bool ArticleManager::GetArticleInfo(const int64 aid, vip_logic::ArticleInfo& article) {
	base_logic::RLockGd lk(lock_);

	bool r = base::MapGet<ARTICLEINFO_MAP,ARTICLEINFO_MAP::iterator,
			int64,vip_logic::ArticleInfo>(article_cache_->article_info_map_,
					aid,article);
	return r;
}

bool ArticleManager::GetArticleInfo(const int64* aid,
		std::map<int64,vip_logic::ArticleInfo>& map) {
	base_logic::RLockGd lk(lock_);
	int32 n = sizeof(aid) / sizeof(int64);
	for (int32 i = 0; i < n; i++) {
		vip_logic::ArticleInfo article;
		bool r = base::MapGet<ARTICLEINFO_MAP,ARTICLEINFO_MAP::iterator,
				int64,vip_logic::ArticleInfo>(article_cache_->article_info_map_,
						aid[i],article);
		if (!r)
			continue;
		map[article.id()] = article;
	}

	return true;
}

bool ArticleManager::GetNewArticle(std::list<vip_logic::ArticleInfo>& list,
		const int32 count) {
	base_logic::RLockGd lk(lock_);
	int32 i = 0;
	article_cache_->article_info_list_.sort(vip_logic::ArticleInfo::cmp);
	std::list<vip_logic::ArticleInfo>::iterator it =
			article_cache_->article_info_list_.begin();
	for (; it != article_cache_->article_info_list_.end(), i < count; it++,i++) {
		list.push_front((*it));
	}

	return true;
}

}
