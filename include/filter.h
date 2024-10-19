#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

void FilterInstructions(const uint64_t & slot , const std::string & blockdata);
void ProcessBlocks(uint64_t start_slot , uint64_t end_slot);

#endif