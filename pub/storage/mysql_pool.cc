#include "mysql_pool.h"

namespace base_logic {

/*
AutoMYSQLEngine::AutoMYSQLEngine() {

}

AutoMYSQLEngine::~AutoMYSQLEngine() {

}*/

MYSQL_Pool::MYSQL_Pool()
:db_pool_lock_(NULL) {}

void MYSQL_Pool::Init(std::list<base::ConnAddr>& addrlist,
				  const int32 db_conn_num/* = 5*/) {
	addrlist_ = addrlist;
	bool r =false;
	InitThreadrw(&db_pool_lock_);
	for (int i = 0; i<db_conn_num;i++){
		base_storage::DBStorageEngine* engine  =
				base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
		if (engine==NULL){
			assert(0);
			LOG_ERROR("create db conntion error");
			continue;
		}

		r = engine->Connections(addrlist_);
		if (!r){
			assert(0);
			LOG_ERROR("db conntion error");
			continue;
		}
	  LOG_ERROR("init db_conn_pool_-----");
		db_conn_pool_.push_back(engine);

	}
}


void MYSQL_Pool::DBConnectionPush(base_storage::DBStorageEngine* engine){
  LOG_ERROR("DBConnectionPush");
	base_logic::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
	LOG_ERROR2("push---db_conn_pool_size[%d]", db_conn_pool_.size());
}

base_storage::DBStorageEngine* MYSQL_Pool::DBConnectionPop(){
  LOG_ERROR("DBConnectionPop");
	if(db_conn_pool_.size()<=0) {
	 LOG_ERROR("db_conn_pool_size 0");
		return NULL;
	}
	base_logic::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    LOG_ERROR2("pop---db_conn_pool_size[%d]", db_conn_pool_.size());
    return engine;
}


void MYSQL_Pool::Dest(){
	base_logic::WLockGd lk(db_pool_lock_);
	while(db_conn_pool_.size()>0){
		base_storage::DBStorageEngine* engine = db_conn_pool_.front();
		db_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
	DeinitThreadrw(db_pool_lock_);
}


}
