#include "filter.h"
#include "rpc_client.h"
#include "rocksdb_handler.h"
#include <nlohmann/json.hpp>  // nlohmann/json 库
#include <iostream>
#include <cstdint>

std::string FilterInstructions(const std::string & blockdata, const std::string & target_programid)
{
    auto json_data = nlohmann::json::parse(blockdata) ;
    const auto & transactions = json_data["result"]["transactions"];
    nlohmann::json filtered_transaction;

    for(const auto & transaction : transactions)
    {
        const auto & message = transaction["transaction"]["message"];
        const auto & meta = transaction["meta"];

        if (message.contains("instructions")) 
        {
            filtered_transaction["instructions"].push_back(message["instructions"]);
        }

        if (meta.contains("innerInstructions")) 
        {
            filtered_transaction["innerInstructions"].push_back(meta["innerInstructions"]);
        }
    }

    return filtered_transaction.dump(); 
}

void ProcessBlocks(uint64_t start_slot, int end_slot, const std::string & target_programid)
{
    for(uint64_t slot = start_slot ; slot <= end_slot ; slot++)
    {
        std::string blockdata = GetBlockData(slot) ;
        if (blockdata.empty())
        {
            std::cerr << "Failed to retrieve block data for slot: " << slot << std::endl;
            continue; 
        }
        std::string filterinstructions = FilterInstructions(blockdata, target_programid);
        StoreBlockData(slot, filterinstructions) ;
    }
}

