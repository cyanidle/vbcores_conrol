#pragma once
#include <stdint.h>
#include <functional>
#include <string_view>

struct Args
{
    int64_t node_id = 77;
    const char* socket = "can0";
    int64_t motor_node_base = 0;
};

Args parse_args(int argc, char** argv);


struct IMotor
{
    virtual int32_t get_encoder() = 0;
    virtual void set_voltage(float value) = 0;
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
void asio_loop(ITeleop* tele, std::function<void()> spin);
