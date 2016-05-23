#include "data_mysql_engine.h"

namespace base_logic {

void DataMYSQLEngine::InitParam(std::list<base::ConnAddr>& addrlist) {
	db_pool_.Init(addrlist);
}

void DataMYSQLEngine::Release() {
	db_pool_.Dest();
}

bool DataMYSQLEngine::WriteData(const int32 type, base_logic::Value* value) {
    bool r = false;
    std::string sql;
    base_logic::DictionaryValue* dict =(base_logic::DictionaryValue*)(value);
    r = dict->GetString(L"sql", &sql);
    if (!r)
        return r;
    base_storage::DBStorageEngine* engine = db_pool_.DBConnectionPop();
    if (engine == NULL) {
          LOG_ERROR("GetConnection Error");
          return false;
      }
     r = engine->SQLExec(sql.c_str());
      if (!r) {
          LOG_ERROR("exec sql error");
          return false;
     }

      db_pool_.DBConnectionPush(engine);

    /*base_db::AutoMysqlCommEngine auto_engine;ß
    base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
    if (engine == NULL) {
        LOG_ERROR("GetConnection Error");
        return false;
    }
   r = engine->SQLExec(sql.c_str());
    if (!r) {
        LOG_ERROR("exec sql error");
        return false;
      LOG_MSG2("[%s]", sql.c_str());
   }*/
    return true;
}


bool DataMYSQLEngine::ReadData(const int32 type, base_logic::Value* value,
        void (*storage_get)(void*, base_logic::Value*)) {
    bool r = false;
    std::string sql;
    base_logic::DictionaryValue* dict =(base_logic::DictionaryValue*)(value);
    r = dict->GetString(L"sql", &sql);
    if (!r)
        return r;

    base_storage::DBStorageEngine* engine = db_pool_.DBConnectionPop();
    if (engine == NULL) {
          LOG_ERROR("GetConnection Error");
          return false;
      }
     engine->Release();
     r = engine->SQLExec(sql.c_str());
      if (!r) {
          LOG_ERROR("exec sql error");
          return false;
     }

      if (storage_get == NULL)
          return r;
      storage_get(reinterpret_cast<void*>(engine), value);

      db_pool_.DBConnectionPush(engine);
    /*base_db::AutoMysqlCommEngine auto_engine;
    base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
    if (engine == NULL) {
        LOG_ERROR("GetConnection Error");
        return false;
    }
    LOG_MSG2("[%s]", sql.c_str());
    r = engine->SQLExec(sql.c_str());

    if (!r) {
        LOG_ERROR("exec sql error");
        return false;
    }

    if (storage_get == NULL)
        return r;
    storage_get(reinterpret_cast<void*>(engine), value);
    */
    return r;
}



}
