//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月29日 Author: kerry

#ifndef GEORGE_VIP_SUBCRIBE_MGR_H__
#define GEORGE_VIP_SUBCRIBE_MGR_H__


#include "vip_basic_info.h"
#include "vip_db.h"
#include "thread/base_thread_lock.h"
#include <map>
#include <string>

typedef std::map<std::string,vip_logic::SubcribeInfo>  SUB_MAP;

namespace vip_logic {

class SubcribeCache {
 public:
	SUB_MAP  user_subcribe_map_;
};

class SubcribeManager {
 public:
	SubcribeManager();
	virtual ~SubcribeManager();

 private:
	void Init();
 public:
	void Init(vip_logic::VIPDB* vip_db);
	int32 GetSubcribeInfo(const std::string& uid,
			const int32 pos,const int32 count,int64** vid);
	bool SetSubcribeInfo(const std::string& uid, const int64 vid);
 private:
	struct threadrw_t*      lock_;
	SubcribeCache*          subcribe_cache_;
	vip_logic::VIPDB*       vip_db_;

};


class SubcribeEngine {
 private:
	static SubcribeManager*   subcribe_mgr_;
	static SubcribeEngine*    subcribe_engine_;

	SubcribeEngine() {}
	virtual ~SubcribeEngine() {}

 public:
	static SubcribeManager* GetSubcribeManager() {
		if (subcribe_mgr_ == NULL)
			subcribe_mgr_ = new SubcribeManager();
		return subcribe_mgr_;
	}

	static SubcribeEngine* GetSubcribeEngine() {
		if (subcribe_engine_ == NULL)
			subcribe_engine_ = new SubcribeEngine();
		return subcribe_engine_;
	}

	static void FreeSubcribeManager() {
		delete subcribe_mgr_;
	}

	static void FreeSubcribeEngine(){
		delete subcribe_engine_;
	}
};
}
#endif
