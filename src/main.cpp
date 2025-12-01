#include <cyphal/cyphal.h>
#include <cyphal/allocators/o1/o1_allocator.h>
#include <cyphal/subscriptions/subscription.h>
#include <cyphal/providers/LinuxCAN.h>
#include <cyphal/node/node_info_handler.h>
#include <uavcan/node/Mode_1_0.h>
#include <uavcan/si/unit/angle/Scalar_1_0.h>
#include <uavcan/primitive/scalar/Real32_1_0.h>
#include <uavcan/primitive/scalar/Natural32_1_0.h>
#include <fmt/core.h>
#include <uavcan/node/Heartbeat_1_0.h>
#include <uavcan/node/Health_1_0.h>
#include <uavcan/node/Mode_1_0.h>
#include "common.hpp"
#include "config.h"


TYPE_ALIAS(Natural32, uavcan_primitive_scalar_Natural32_1_0)
TYPE_ALIAS(Real32, uavcan_primitive_scalar_Real32_1_0)
TYPE_ALIAS(HBeat, uavcan_node_Heartbeat_1_0)

void cyphal_node_unique_id(uint32_t& a, uint32_t& b, uint32_t& c)
{
    a = 1;
    b = 2;
    c = 3;
}

static uint32_t seconds() {
    static auto begin = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();
}

static void heartbeat(CyphalInterface& iface) {
    static CanardTransferID hbeat_transfer_id = 0;
    HBeat::Type heartbeat_msg = {};
    heartbeat_msg.uptime = seconds();
    heartbeat_msg.health = {uavcan_node_Health_1_0_NOMINAL};
    heartbeat_msg.mode = {uavcan_node_Mode_1_0_OPERATIONAL};
    iface.send_msg<HBeat>(&heartbeat_msg, uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_, &hbeat_transfer_id);
}

static constexpr CanardPortID ENCODER_PORT = 7100;
static constexpr CanardPortID VOLTAGE_PORT = 5800;

struct Motor final :
    private AbstractSubscription<Natural32>,
    public IMotor
{
    Motor(std::shared_ptr<CyphalInterface> iface, const Args* args, int index) :
        AbstractSubscription<Natural32>(iface, CanardPortID(ENCODER_PORT + args->motor_node_base + index)),
        index(index),
        args(args)
    {}

    Motor(Motor&&) = delete;

    std::atomic<int32_t> encoder_value = 0;
    int index;
    const Args* args;
    CanardTransferID _transfer_id = 0;

    int32_t get_encoder() override {
        return encoder_value;
    }

    void set_target_speed(float value) override {
        Real32::Type msg{};
        msg.value = value * args->max_voltage;
        interface->send_msg<Real32>(&msg, VOLTAGE_PORT + args->motor_node_base + index, &_transfer_id);
    }
private:
    void handler(const Natural32::Type& msg, CanardRxTransfer* _) override {
        encoder_value = msg.value;
    }
};

int main(int argc, char** argv) try
{
    Args args = parse_args(argc, argv);

    std::shared_ptr<CyphalInterface> cyphal = CyphalInterface::create_heap<LinuxCAN, O1Allocator>(
        args.node_id,
        args.socket,    // SocketCAN
        200,            // длина очереди
        DEFAULT_CONFIG  // дефолтные настройки для линукса, объявлены в библиотеке
    );

    Motor motors[4] = {
        {cyphal, &args, 0},
        {cyphal, &args, 1},
        {cyphal, &args, 2},
        {cyphal, &args, 3},
    };

    IMotor* imotors[] = {
        motors + 0,
        motors + 1,
        motors + 2,
        motors + 3,
    };

    ITeleop* tele = make_teleop(imotors);

    uint64_t vcs_id = std::stoull(std::string(VERSION_HASH).substr(0, 16), nullptr, 16);

    NodeInfoReader reader(cyphal,
        "org.bfu.vb_control",
        uavcan_node_Version_1_0{1, 0},
        uavcan_node_Version_1_0{1, 0},
        uavcan_node_Version_1_0{1, 0},
        vcs_id);

    AsioLoopParams params;
    params.heartbeat = [&]{
        heartbeat(*cyphal);
    };

    cyphal->start_threads();

    asio_loop(tele, params);
} catch(std::exception& e) {
    fmt::println(stderr, "ERROR: {}", e.what());
    return 1;
}
