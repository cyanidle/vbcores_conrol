#pragma once
#include <stdint.h>
#include <functional>

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

struct ITeleop
{
    virtual void handle_press(int key) = 0;
};

ITeleop* make_teleop(IMotor** motors);
void asio_loop(ITeleop* tele, std::function<void()> spin);
