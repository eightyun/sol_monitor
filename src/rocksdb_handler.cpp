#include "rocksdb_handler.h"
#include "rocksdb/db.h"
#include <iostream>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <mutex>

rocksdb::DB * db = nullptr ;
rocksdb::Options options ;
extern std::mutex mtx; 

void InitRocksDB()
{
    if (db != nullptr) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "RocksDB is already initialized" << std::endl;
        return;
    }

    options.create_if_missing = true ;
    rocksdb::Status status = rocksdb::DB::Open(options , "/tmp/testdb" , &db) ;
    if (!status.ok())
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "Unable to open RocksDB: " << status.ToString() << std::endl;
    }
}

void StoreBlockData(const std::string & key , const std::string & blockdata)
{
    rocksdb::Status status = db->Put(rocksdb::WriteOptions() , key , blockdata);
    if(!status.ok())
    {
        std::lock_guard<std::mutex> lock(mtx);
         std::cerr << "Failed to store block data: " << status.ToString() << std::endl;
    }
}

nlohmann::json GetStoredBlockData(const uint64_t & slot)
{
    std::string value ;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions() , std::to_string(slot) , &value);
    if(!status.ok()) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "Failed to get block data" << status.ToString() << std::endl;
        return nlohmann::json();  
    }

    if (!value.empty()) 
    {
        try 
        {
            return nlohmann::json::parse(value);
        } 
        catch (const nlohmann::json::parse_error & e) 
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }

    return nlohmann::json();  
}

void CloseRocksDB()
{
    if (db) 
    {
        delete db;
        db = nullptr;
    }
}

