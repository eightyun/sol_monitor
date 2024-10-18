#ifndef ROCKSDB_HANDLER_H
#define ROCKSDB_HANDLER_H

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

void InitRocksDB();
void StoreBlockData(uint64_t slot , const std::string & blockdata) ;
nlohmann::json GetStoredBlockData(uint64_t slot) ;
void CloseRocksDB() ;

#endif