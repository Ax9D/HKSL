#include <FSUtil.h>
#include <Util.h>

#include <cstdio>
#include <cstring>
#include <format>
#include <cassert>

namespace HKSL {
std::string read_to_string(const char* path) {
    FILE* file = fopen(path, "r");
    if(!file) {
        HKSL_ERROR(std::format("Failed to read file: {}", strerror(errno)));
    }

    fseek(file, 0, SEEK_END);
    size_t nbytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string str(nbytes, '\0');
    assert(fread(str.data(), 1, nbytes, file) == nbytes);

    return str;
}
}