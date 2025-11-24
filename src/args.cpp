#include "common.hpp"
#define TAPKI_IMPLEMENTATION
#include "tapki.h"

Args parse_args(int argc, char** argv)
{
    Arena* arena = ArenaCreate(1024);
    Args args;

    CLI cli[] = {
        {.name="node_id", .data=&args.node_id, .help="Override default node id [default=101]"},
        {},
    };

    if (ParseCLI(cli, argc, argv) != 0) {
        std::exit(1);
    }

    ArenaFree(arena);
    return args;
}