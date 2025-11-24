#include <cyphal/cyphal.h>
#include <cyphal/allocators/o1/o1_allocator.h>
#include <cyphal/subscriptions/subscription.h>
#include <cyphal/providers/LinuxCAN.h>
#include "common.hpp"

int main(int argc, char** argv) try
{
    Args args = parse_args(argc, argv);
    const int NODE_ID = 101;

    auto cyphal_interface = std::shared_ptr<CyphalInterface>(CyphalInterface::create_heap<LinuxCAN, O1Allocator>(
        NODE_ID,
        "can0",         // SocketCAN
        200,            // длина очереди
        DEFAULT_CONFIG  // дефолтные настройки для линукса, объявлены в библиотеке
    ));
} catch(std::exception& e) {
    
}