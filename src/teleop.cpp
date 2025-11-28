#include "common.hpp"
#include "curses.h"
#include <cstdlib>
#include <fmt/core.h>

static void norm(float& x) {
    if (std::abs(x) > 1.0f) {
        x = x > 0 ? 1.0f : -1.0f;
    }
}

struct Teleop final : ITeleop
{
    IMotor** motors;
    DriveTarget target = {};
    std::string msg;
    bool shift = false;

    Teleop(IMotor** mot) : motors(mot) {}

    DriveTarget get_target() override {return target;}

    void apply_target() {
        for (int i = 0; i < 4; ++i) {
            float sign = (i % 1) ? -1.f : 1.f;
            float move = target.x * 0.7 * sign;
            float rot = target.x * 0.3 * sign;
            float res = move + rot;
            if (res < std::numeric_limits<float>::epsilon()) {
                res = 0;
            }
            motors[i]->set_target_speed(res);
        }
    }

    void handle_press(int ch) override {
        msg = fmt::format("Last: '{}'", char(ch));
        switch (ch) {
        case 'w': case 'W': {
            target.x += 0.1;
            break;
        }
        case 's': case 'S': {
            target.x -= 0.1;
            break;
        }
        case 'a': case 'A': {
            target.th += 0.1;
            break;
        }
        case 'd': case 'D': {
            target.th -= 0.1;
            break;
        }
        case 'q': case 'Q': {
            target.x += 0.1;
            target.th += 0.1;
            break;
        }
        case 'e': case 'E': {
            target.x += 0.1;
            target.th -= 0.1;
            break;
        }
        case ' ': {
            target = {};
            break;
        }
        }
        norm(target.x);
        norm(target.th);
        apply_target();
    }

    virtual std::string extra_msg() override {
        return msg;
    }
};

ITeleop* make_teleop(IMotor** motors)
{
    return new Teleop{motors};
}