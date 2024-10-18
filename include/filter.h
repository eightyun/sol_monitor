#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

std::string FilterInstructions(const std::string & blockdata, const std::string & target_programid);
void ProcessBlocks(uint64_t start_slot, int end_slot, const std::string & target_programid);

#endif