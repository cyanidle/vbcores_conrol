#pragma once
#include <stdint.h>
#include <functional>
#include <string_view>
#include <string>

struct Args
{
    int64_t node_id = 77;
    float max_voltage = 20;
    std::string socket = "can0";
    int64_t motor_node_base = 0;
};

Args parse_args(int argc, char** argv);


struct IMotor
{
    virtual int32_t get_encoder() = 0;
    virtual void set_target_speed(float value) = 0;
};

struct DriveTarget {
    float x;
    float th;

    bool operator<=>(const DriveTarget&) const = default;
};

struct ITeleop
{
    virtual DriveTarget get_target() = 0;
    virtual void handle_press(int key) = 0;
    virtual std::string extra_msg() = 0;
};

ITeleop* make_teleop(IMotor** motors);

struct AsioLoopParams
{
    std::function<void()> spin;
    std::function<void()> heartbeat;
};

void asio_loop(ITeleop* tele, AsioLoopParams const& params);
