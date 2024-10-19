#ifndef ROCKSDB_HANDLER_H
#define ROCKSDB_HANDLER_H

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

void InitRocksDB();
void StoreBlockData(const std::string & key , const std::string & blockdata) ;
nlohmann::json GetStoredBlockData(const uint64_t & slot) ;
void CloseRocksDB() ;

#endif