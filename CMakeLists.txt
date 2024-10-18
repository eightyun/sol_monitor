cmake_minimum_required(VERSION 3.17.0)
project(solana_monitor)

set(CMAKE_CXX_STANDARD 17)

# 查找 nlohmann/json 安装的库
find_package(nlohmann_json 3.11.3 REQUIRED)

# 添加libcurl依赖
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# 添加include
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加RocksDB依赖
find_library(ROCKSDB_LIB rocksdb)

# 源文件
set(SOURCES
    src/main.cpp
    src/rpc_client.cpp
    src/rocksdb_handler.cpp
    src/filter.cpp
)

# 编译可执行文件
add_executable(solana_monitor ${SOURCES})

# 链接库
target_link_libraries(solana_monitor ${CURL_LIBRARIES} ${ROCKSDB_LIB} nlohmann_json::nlohmann_json zstd z lz4 snappy bz2)
