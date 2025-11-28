#include "common.hpp"

#include <argparse/argparse.hpp>

Args parse_args(int argc, char** argv)
{
    argparse::ArgumentParser parser(argv[0]);
    Args args;

    parser
        .add_argument("--node", "-n")
        .help("Override default node id")
        .default_value(args.node_id)
        .scan<'i', int64_t>();
    parser
        .add_argument("--socket", "-s")
        .help("Can socket to use")
        .default_value(args.socket);
    parser
        .add_argument("--motors", "-m")
        .help("Motor node_id start (4 motors)")
        .default_value(args.motor_node_base)
        .scan<'i', int64_t>();

    parser.parse_args(argc, argv);

    args.motor_node_base = parser.get<int64_t>("motors");
    args.socket = parser.get("socket");
    args.node_id = parser.get<int64_t>("node");

    return args;
}