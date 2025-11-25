#include <cyphal/cyphal.h>
#include <cyphal/allocators/o1/o1_allocator.h>
#include <cyphal/subscriptions/subscription.h>
#include <cyphal/providers/LinuxCAN.h>
#include <uavcan/node/Mode_1_0.h>
#include <uavcan/si/unit/angle/Scalar_1_0.h>
#include <uavcan/primitive/scalar/Real32_1_0.h>
#include <uavcan/primitive/scalar/Natural32_1_0.h>
#include <fmt/core.h>
#include "common.hpp"
#define TAPKI_IMPLEMENTATION
#include "tapki.h"

Args parse_args(int argc, char** argv)
{
    Arena* arena = ArenaCreate(2048);
    Args args;

    Str sock{};

    CLI cli[] = {
        {.name="--node,-n", .data=&args.node_id, .help=F("Override default node id [default=%li]", args.node_id).d, .int64=true},
        {.name="--socket,-s", .data=&sock, .help=F("Can socket to use [default=%s]", args.socket).d},
        {.name="--motors,-m", .data=&args.motor_node_base, .help=F("Motor node_id start (4 motors) [default=%li]", args.motor_node_base).d},
        {},
    };

    if (ParseCLI(cli, argc, argv) != 0) {
        std::exit(1);
    }

    if (sock.size) {
        args.socket = sock.d;
    }

    return args;
}

TYPE_ALIAS(Natural32, uavcan_primitive_scalar_Natural32_1_0)
TYPE_ALIAS(Real32, uavcan_primitive_scalar_Real32_1_0)

static constexpr CanardPortID ENCODER_PORT = 7100;
static constexpr CanardPortID VOLTAGE_PORT = 5800;

struct Motor final :
    private AbstractSubscription<Natural32>,
    public IMotor
{
    Motor(std::shared_ptr<CyphalInterface> iface, int64_t base, int index) :
        AbstractSubscription<Natural32>(iface, CanardPortID(ENCODER_PORT + base + index)),
        base(base),
        index(index)
    {}

    Motor(Motor&&) = delete;

    std::atomic<int32_t> encoder_value = 0;
    int64_t base;
    int index;


    int32_t get_encoder() override {
        return encoder_value;
    }

    void set_voltage(float value) override {
        Real32::Type msg{};
        msg.value = value;
        static CanardTransferID _transfer_id;
        interface->send_msg<Real32>(&msg, VOLTAGE_PORT + base + index, &_transfer_id);
    }
private:
    void handler(const Natural32::Type& msg, CanardRxTransfer* _) override {
        encoder_value = msg.value;
    }
};

int main(int argc, char** argv) try
{
    Args args = parse_args(argc, argv);

    auto cyphal_interface = CyphalInterface::create_heap<LinuxCAN, O1Allocator>(
        args.node_id,
        args.socket,    // SocketCAN
        200,            // длина очереди
        DEFAULT_CONFIG  // дефолтные настройки для линукса, объявлены в библиотеке
    );

    Motor motors[4] = {
        {cyphal_interface, args.motor_node_base, 0},
        {cyphal_interface, args.motor_node_base, 1},
        {cyphal_interface, args.motor_node_base, 2},
        {cyphal_interface, args.motor_node_base, 3},
    };

    IMotor* imotors[] = {
        motors + 0,
        motors + 1,
        motors + 2,
        motors + 3,
    };

    ITeleop* tele = make_teleop(imotors);

    cyphal_interface->start_threads();

    asio_loop(tele, [&]{
        cyphal_interface->loop();
    });
} catch(std::exception& e) {
    fmt::println(stderr, "ERROR: {}", e.what());
}