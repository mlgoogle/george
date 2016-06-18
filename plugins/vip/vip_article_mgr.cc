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
	vip_db_->FectchArticleInfo(0,article_cache_->article_info_map_,
			article_cache_->article_info_list_,article_cache_->article_info_vec_);
	std::sort(article_cache_->article_info_vec_.begin(),
			article_cache_->article_info_vec_.end(),vip_logic::ArticleInfo::cmp);
	article_cache_->article_info_list_.sort(vip_logic::ArticleInfo::cmp);
	SetVIPArticle();
}

void ArticleManager::UpdateArticle() {
	/*base_logic::WLockGd lk(lock_);
	vip_db_->FectchArticleInfo(100,article_cache_->article_info_map_,
			article_cache_->article_info_list_,article_cache_->article_info_vec_);
	std::sort(article_cache_->article_info_vec_.begin(),
			article_cache_->article_info_vec_.end(),vip_logic::ArticleInfo::cmp);
	article_cache_->article_info_list_.sort(vip_logic::ArticleInfo::cmp);
	SetVIPArticle();
	LOG_MSG2("map %d, vec %d ,list %d",article_cache_->article_info_map_.size(),
			article_cache_->article_info_vec_.size(),
			article_cache_->article_info_list_.size());*/
}

void ArticleManager::SetVIPArticle() {
	for (ARTICLEINFO_MAP::iterator it = article_cache_->article_info_map_.begin();
			it != article_cache_->article_info_map_.end(); it++) {
		vip_logic::ArticleInfo article = it->second;
		// 0 直播  1博文
		if (article.type() == 1) {
			VIPARTICLE_VEC::iterator itr =
					article_cache_->vip_article_info_vec_.find(article.own_id());
			ARTICLEINFO_VEC vec;
			if (itr == article_cache_->vip_article_info_vec_.end()) {
				vec.push_back(article);

			} else {
				vec = itr->second;
				vec.push_back(article);
			}

			article_cache_->vip_article_info_vec_[article.own_id()] = vec;
		} else {
			VIPARTICLE_VEC::iterator itr =
					article_cache_->vip_live_info_vec_.find(article.own_id());
			ARTICLEINFO_VEC vec;
			if (itr == article_cache_->vip_live_info_vec_.end()) {
				vec.push_back(article);

			} else {
				vec = itr->second;
				vec.push_back(article);
			}

			article_cache_->vip_live_info_vec_[article.own_id()] = vec;
		}
	}
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
		list.push_back((*it));
	}

	return true;
}

bool ArticleManager::GetNewArticle(std::list<vip_logic::ArticleInfo>& list,const int32 pos,
			const int32 count) {
	base_logic::RLockGd lk(lock_);
	std::sort(article_cache_->article_info_vec_.begin(),
				article_cache_->article_info_vec_.end(),vip_logic::ArticleInfo::cmp);

	int32 index = 0;
	while (index < pos)
		index++;

	int32 vec_size = article_cache_->article_info_vec_.size();

	while (index < vec_size && list.size() < count) {
		list.push_back(article_cache_->article_info_vec_[index]);
		index++;
	}
	return true;
}

bool ArticleManager::GetVIPNewArticle(const int64* uid,const int32 n,
			std::map<int64,vip_logic::ArticleInfo>& map) {
	base_logic::RLockGd lk(lock_);
	bool r = false;
	for (int32 i = 0; i < n; i++) {
		ARTICLEINFO_VEC vec;
		r = base::MapGet<VIPARTICLE_VEC,VIPARTICLE_VEC::iterator,
				int64,ARTICLEINFO_VEC>(article_cache_->vip_article_info_vec_,
						uid[i],vec);
		if(r) {
			vip_logic::ArticleInfo article = vec[0];
			map[uid[i]] = article;
		}
	}

	return true;
}


bool ArticleManager::GetVIPArticle(const int64 vid,
			std::list<vip_logic::ArticleInfo>& list,const int32 pos,
			const int32 count,const int8 flag) {
	base_logic::RLockGd lk(lock_);
	/*if (flag == 1) */{
		ARTICLEINFO_VEC vec;
		bool r = base::MapGet<VIPARTICLE_VEC,VIPARTICLE_VEC::iterator,
					int64,ARTICLEINFO_VEC>(article_cache_->vip_article_info_vec_,
							vid,vec);
		if (r)
			GetVIPArticleT(list, pos, count,vec);
	}

	/*if(flag == 2) */{
		ARTICLEINFO_VEC vec;
		bool r = base::MapGet<VIPARTICLE_VEC,VIPARTICLE_VEC::iterator,
					int64,ARTICLEINFO_VEC>(article_cache_->vip_live_info_vec_,
							vid,vec);
		if (r)
			GetVIPArticleT(list, pos, count,vec);
	}

	/*
	std::sort(vec.begin(),vec.end(),vip_logic::ArticleInfo::cmp);

	int32 index = 0;
	while (index < pos)
		index++;

	int32 size = vec.size() > count ? count : vec.size();
	while (index < size) {
		list.push_back(vec[index]);
		index++;
	}
	*/
	return true;
}

bool ArticleManager::GetVIPArticleT(std::list<vip_logic::ArticleInfo>& list,const int32 pos,
		const int32 count, ARTICLEINFO_VEC& vec) {

	std::sort(vec.begin(),vec.end(),vip_logic::ArticleInfo::cmp);

	int32 index = 0;

	while (index < pos)
		index++;

	int32 size = vec.size() > count ? count : vec.size();
	while (index < size) {
		list.push_back(vec[index]);
		index++;
	}
	return true;
}


}
