#include "rpc_client.h" 
#include "rocksdb_handler.h"
#include "filter.h"
#include <iostream>

int main()
{
    auto start_time = std::chrono::high_resolution_clock::now(); 

// ----------------------------------------------------------------

    InitRocksDB() ;

    uint64_t start_slot ;
    std::cin >> start_slot ;

    uint64_t end_solt ;
    std::cin >> end_solt ;

    ProcessBlocks(start_slot , end_solt) ;

    CloseRocksDB() ;

// ----------------------------------------------------------------

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Program executed in " << elapsed.count() << " seconds." << std::endl;

    return 0 ; 
}