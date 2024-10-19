#include "filter.h"
#include "rpc_client.h"
#include "rocksdb_handler.h"
#include <nlohmann/json.hpp>  // nlohmann/json 库
#include <iostream>
#include <cstdint>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <condition_variable>

std::mutex mtx ; 
std::mutex cv_mtx ;                     // 用于保护条件变量的互斥锁
std::condition_variable cv ;            // 条件变量，用于控制并发数量
int active_threads = 0 ;                // 当前活动的线程数量
const int max_concurrent_threads = 16 ; // 最大并发线程数量

void FilterInstructions(const uint64_t & slot , const std::string & blockdata)
{
    auto json_data = nlohmann::json::parse(blockdata) ;
    const auto & transactions = json_data["result"]["transactions"];

    if (transactions.size() == 0)
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "No transactions found for slot: " << slot << std::endl;
        return;
    }

    for(size_t tx_count = 0 ; tx_count < transactions.size() ; tx_count++)
    {
        const auto & transaction = transactions[tx_count] ;
        const auto & message = transaction["transaction"]["message"] ;
        const auto & meta = transaction["meta"] ;

        if (message.contains("instructions"))
        {
            nlohmann::json instructions_array = nlohmann::json::array();
            for (const auto& instruction : message["instructions"])
            {
                instructions_array.push_back(instruction);
            }

            std::string instruction_key = std::to_string(slot) + ":" + std::to_string(tx_count) + ":" + "instruction";
            StoreBlockData(instruction_key, instructions_array.dump());
        }

        if(meta.contains("innerInstructions"))
        {
            nlohmann::json inner_array = nlohmann::json::array();
            for(const auto & inner_instruction_set : meta["innerInstructions"])
            {
                for (const auto& inner_instruction : inner_instruction_set["instructions"])
                {
                    inner_array.push_back(inner_instruction);
                }
            }

            std::string inner_key = std::to_string(slot) + ":" + std::to_string(tx_count) + ":" + "inner";
            StoreBlockData(inner_key, inner_array.dump());
        }
    }
}

void ProcessBlocks(uint64_t start_slot, uint64_t end_slot)
{
    std::vector<std::future<void>> futures;

    for (uint64_t slot = start_slot ; slot <= end_slot ; slot++)
    {
        // 限制并发数量，控制同时执行的线程数
        std::unique_lock<std::mutex> lock(cv_mtx);
        cv.wait(lock, [] { return active_threads < max_concurrent_threads; });
        active_threads++;
        lock.unlock();

        futures.emplace_back(std::async(std::launch::async, [slot]()
        {
            int wait_time = 100; 
            std::string blockdata;

            while (true)
            {
                blockdata = GetBlockData(slot);
                if (!blockdata.empty() && nlohmann::json::accept(blockdata))
                {
                    break; 
                }

                std::lock_guard<std::mutex> lock(mtx);
                std::cerr << "Failed to retrieve valid block data for slot: " << slot << ", retrying..." << std::endl;

                std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
                wait_time = std::min(wait_time * 2, 5000); 
            }

            try
            {
                FilterInstructions(slot, blockdata);
            }
            catch (const std::exception& e)
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cerr << "Exception occurred while filtering instructions for slot: " << slot << " - " << e.what() << std::endl;
            }

            {
                std::lock_guard<std::mutex> lock(cv_mtx);
                active_threads--;
            }
            cv.notify_one();
        }));
    }

    // 等待所有异步任务完成
    for (auto& future : futures)
    {
        future.get();
    }
}

