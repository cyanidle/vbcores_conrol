#pragma once
#include <stdint.h>

struct Args
{
    int64_t node_id = 101;
};

Args parse_args(int argc, char** argv);