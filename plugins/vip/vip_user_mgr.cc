#include "vip_user_mgr.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include "thread/base_thread_lock.h"
#include <algorithm>

namespace vip_logic {


VIPUserManager* VIPUserEngine::vip_user_mgr_ = NULL;
VIPUserEngine* VIPUserEngine::vip_user_engine_ = NULL;


VIPUserManager::VIPUserManager()
:vip_db_(NULL){
	vip_user_cache_ = new VIPUserCache();
	Init();
}

VIPUserManager::~VIPUserManager() {
	DeinitThreadrw(lock_);
}

void VIPUserManager::Init() {
	InitThreadrw(&lock_);
}

void VIPUserManager::Init(vip_logic::VIPDB* vip_db) {
	vip_db_ = vip_db;
	vip_db_->FectchVIPUserInfo(vip_user_cache_->vip_user_info_,
			vip_user_cache_->vip_user_vec_);
	std::sort(vip_user_cache_->vip_user_vec_.begin(),
			vip_user_cache_->vip_user_vec_.end(),vip_logic::VIPUserInfo::cmp);
}

bool VIPUserManager::GetVIPUserInfo(const int64 vid,
		vip_logic::VIPUserInfo& vip) {
	base_logic::RLockGd lk(lock_);

	bool r = base::MapGet<VIPUSERINFO_MAP,VIPUSERINFO_MAP::iterator,
			int64,vip_logic::VIPUserInfo>(vip_user_cache_->vip_user_info_,
					vid,vip);
	return r;
}

bool VIPUserManager::GetVIPUserInfo(const int64* uid,const int32 n,
			std::map<int64, vip_logic::VIPUserInfo> & map) {
	base_logic::RLockGd lk(lock_);
	for (int32 i = 0; i < n; i++) {
		vip_logic::VIPUserInfo vip_info;
		bool r = base::MapGet<VIPUSERINFO_MAP,VIPUSERINFO_MAP::iterator,
				int64,vip_logic::VIPUserInfo>(vip_user_cache_->vip_user_info_,
						uid[i],vip_info);
		if (!r)
			continue;
		map[vip_info.id()] = vip_info;
	}

	return true;
}


bool VIPUserManager::GetHotVIPUser(std::list<vip_logic::VIPUserInfo>& list,const int32 pos,
				const int32 count) {

	base_logic::RLockGd lk(lock_);
	std::sort(vip_user_cache_->vip_user_vec_.begin(),
			vip_user_cache_->vip_user_vec_.end(),vip_logic::VIPUserInfo::cmp);

	int32 index = 0;
	while (index < pos)
		index++;

	while (index < count) {
		list.push_back(vip_user_cache_->vip_user_vec_[index]);
		index++;
	}
	return true;
}


}
