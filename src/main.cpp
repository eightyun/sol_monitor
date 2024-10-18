#include "rpc_client.h" 
#include "rocksdb_handler.h"
#include "filter.h"
#include <iostream>

int main()
{
    InitRocksDB() ;

    uint64_t start_slot ;
    std::cin >> start_slot ;

    uint64_t end_solt ;
    std::cin >> end_solt ;

    std::string target_programid ;
    std ::cin >> target_programid ;

    ProcessBlocks(start_slot , end_solt , target_programid) ;

    CloseRocksDB() ;

    return 0 ; 
}