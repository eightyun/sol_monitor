#include "rocksdb_handler.h"
#include "rocksdb/db.h"
#include <iostream>
#include <cstdint>
#include <nlohmann/json.hpp>

rocksdb::DB * db ;
rocksdb::Options options ;

void InitRocksDB()
{
    options.create_if_missing = true ;
    rocksdb::Status status = rocksdb::DB::Open(options , "/tmp/testdb" , &db) ;
    if (!status.ok()) std::cerr << "Unable to open RocksDB" << std::endl;
}

void StoreBlockData(uint64_t slot , const std::string & blockdata)
{
    rocksdb::Status status = db->Put(rocksdb::WriteOptions() , std::to_string(slot) , blockdata);
    if(!status.ok()) std::cerr << "Failed to store block data" << std::endl;
}

nlohmann::json GetStoredBlockData(uint64_t slot)
{
    std::string value ;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions() , std::to_string(slot) , &value);
    if(!status.ok()) 
    {
        std::cerr << "Failed to get block data" << std::endl;
        return nlohmann::json();  
    }
    
    if (!value.empty()) 
    {
        return nlohmann::json::parse(value);
    }

    return nlohmann::json();  
}

void CloseRocksDB()
{
    delete db ;
}

