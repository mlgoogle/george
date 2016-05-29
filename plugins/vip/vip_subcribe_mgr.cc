#include "vip_subcribe_mgr.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include "thread/base_thread_lock.h"
#include <algorithm>

namespace vip_logic {


SubcribeManager* SubcribeEngine::subcribe_mgr_ = NULL;
SubcribeEngine* SubcribeEngine::subcribe_engine_ = NULL;


SubcribeManager::SubcribeManager()
:vip_db_(NULL) {
	subcribe_cache_ = new SubcribeCache();
	Init();
}

SubcribeManager::~SubcribeManager() {
	DeinitThreadrw(lock_);
}

void SubcribeManager::Init(){
	InitThreadrw(&lock_);
}

void SubcribeManager::Init(vip_logic::VIPDB* vip_db) {
	vip_db_ = vip_db;
	vip_db_->FectchSubcribeInfo(subcribe_cache_->user_subcribe_map_);
}

int32 SubcribeManager::GetSubcribeInfo(const std::string& uid,
		const int32 pos,const int32 count,
		int64** vid) {

	vip_logic::SubcribeInfo subcribe_info;

	bool r = base::MapGet<SUB_MAP,SUB_MAP::iterator,std::string,vip_logic::SubcribeInfo>(
			subcribe_cache_->user_subcribe_map_,uid,subcribe_info);


	int32 index = 0;
	while (index < pos)
		index++;

	if (index > subcribe_info.subcribe_size()) //超出极限
		return false;

	int32 surplus = subcribe_info.subcribe_size() - index;
	int32 tsize = count > surplus ? surplus: count;
	return subcribe_info.get_vid(index,tsize,vid);

}




}
