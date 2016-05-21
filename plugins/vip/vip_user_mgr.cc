#include "vip_user_mgr.h"

namespace vip_logic {


VIPUserManager* VIPUserEngine::vip_user_mgr_ = NULL;
VIPUserEngine* VIPUserEngine::vip_user_engine_ = NULL;


VIPUserManager::VIPUserManager() {
	vip_user_cache_ = new VIPUserCache();
	Init();
}

VIPUserManager::~VIPUserManager() {
	DeinitThreadrw(lock_);
}

VIPUserManager::Init(vip_logic::VIPDB* vip_db) {
	InitThreadrw(&lock_);
	vip_db_ = vip_db;
}

bool VIPUserManager::GetVIPUserInfo(const int64 vid,
		vip_logic::VIPUserInfo& vip) {

	bool r = base::MapGet<VIPUSERINFO_MAP,VIPUSERINFO_MAP::iterator,
			int64,vip_logic::VIPUserInfo>(vip_user_cache_->vip_user_info_,
					vid,vip);
	return r;
}




}
