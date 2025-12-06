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

struct DriveTarget;
struct ITeleop;

struct Io
{
    virtual void spawn(unsigned millis, std::function<void()> func) = 0;
    virtual void dispatch(std::function<void()> func) = 0;
    virtual void set_teleop(ITeleop* tele) = 0;
    virtual void print(DriveTarget const& target, std::string_view msg) = 0;
    virtual void loop() = 0;
};

struct IMotor
{
    virtual void set_target_speed(float value) = 0;
    virtual void set_encoder_callback(std::function<void(int32_t)> cb) = 0;
};

struct DriveTarget {
    float x;
    float th;

    bool operator<=>(const DriveTarget&) const = default;
};

struct ITeleop
{
    virtual void handle_press(int key) = 0;
};

Io* make_asio();
ITeleop* make_teleop(Io*, IMotor** motors);
