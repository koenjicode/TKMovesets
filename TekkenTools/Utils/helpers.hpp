#pragma once

#include <string>

namespace Helpers
{
    // Gets the current date format in 'hour:minutes day/month/year'
    std::string currentDateTime();

    // In a (moveset) list, can convert ptr members of every structure in the list into offsets
    void ConvertPtrsToOffset(void* listAddr, uint64_t to_substract, uint64_t struct_size, uint64_t amount);

    // Returns true if a string ends with [suffix]
    bool endsWith(std::string_view str, std::string_view suffix);

    // Converts filename into displayable name
    std::string getMovesetNameFromFilename(std::string filename);
}