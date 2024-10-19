#include "rpc_client.h"
#include <curl/curl.h>
#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <mutex>

extern std::mutex mtx; 

// 回调函数 用于处理 libcurl 获取的数据
size_t WriteCallback(void * contents , size_t size , size_t nmemb , void * userp)
{
    ((std::string*)userp)->append((char*)contents , size * nmemb) ;
    return size * nmemb ;
}

// 通过libcurl从RPC获取区块数据
std::string GetBlockData(const uint64_t & slot)
{
    CURL * curl ;
    CURLcode res ;
    curl = curl_easy_init();
    std::string readBuffer ;

    if(curl)
    {
        std::string url  = "https://docs-demo.solana-mainnet.quiknode.pro/"; 
        std::string data = "{\"jsonrpc\":\"2.0\", \"id\":1, \"method\":\"getBlock\", \"params\":[" 
                            + std::to_string(slot) + 
                            ", {\"encoding\":\"jsonParsed\", \"maxSupportedTransactionVersion\":0,\"transactionDetails\":\"full\", \"rewards\":false}]}";

        curl_easy_setopt(curl , CURLOPT_URL , url.c_str());
        curl_easy_setopt(curl , CURLOPT_POSTFIELDS , data.c_str());

        curl_easy_setopt(curl , CURLOPT_WRITEFUNCTION , WriteCallback);
        curl_easy_setopt(curl , CURLOPT_WRITEDATA , &readBuffer);

        curl_easy_setopt(curl , CURLOPT_TIMEOUT, 10L); // 设置超时为10秒
        curl_easy_setopt(curl , CURLOPT_FAILONERROR, 1L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) 
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "curl_easy_init() failed" << std::endl;
    }

    return readBuffer ;
}